/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with this program; if not, write to the Free Software Foundation, Inc.,    *
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                *
 ******************************************************************************/

#include "exception.h"
#include "firfilter.h"

#include <fftw3.h>

#include <complex>
#include <climits>

#include <QFile>
#include <QString>
#include <QVector>

// the complex multiplication needs four multiplications which should also be
// possible in 3, but it does not seem to matter
typedef std::complex<double> Complex;

// We fail horribly if the complex type used here has a different size than the
// complex type from fftw (fftw_complex)
#define CASSERT(predicate) \
    extern char constraint_violated[2*((predicate)!=0)-1];
CASSERT(sizeof (Complex) == sizeof (fftw_complex))

namespace syl
{

namespace
{

// Data array that is based on fftw_allow/fftw_free
//
// Needed because otherwise it can not assured that the alignment for the
// fftw_execute_dft_r2c/fftw_execute_dft_c2r functions is consistent
//
// Data is copied directly (no QSharedData) because no accessor is used
// for the data field; instances are only copied if FirFilterPrivate
// itself is copied which implies that write access is going to occur in
// FirFilter#process()
template <typename T>
class FftwData
{
public:
    FftwData() :
        size (0),
        fftwData (NULL)
    {
    }

    FftwData (unsigned size) :
        size (size),
        fftwData (reinterpret_cast<T*> (fftw_malloc (sizeof (T) * size)))
    {
        memset (fftwData, 0, sizeof (T) * size);
    }

    ~FftwData()
    {
        fftw_free (fftwData);
    }

    FftwData (const FftwData &other) :
        size (other.size),
        fftwData (reinterpret_cast<T*> (fftw_malloc (sizeof (T) * size)))
    {
        memcpy (fftwData, other.fftwData, sizeof (T) * size);
    }

    FftwData &operator= (const FftwData &other)
    {
        if (this == &other)
            return *this;

        size = other.size;
        fftw_free (fftwData);
        fftwData = reinterpret_cast<T*> (fftw_malloc (sizeof (T) * size));
        memcpy (fftwData, other.fftwData, sizeof (T) * size);

        return *this;
    }

    T *data()
    {
        return fftwData;
    }

    const T *data() const
    {
        return fftwData;
    }

private:
    unsigned size;
    T *fftwData;
};

// Emulate boost::shared_ptr with a custom delete function by using
// Qt::QExplicitlySharedDataPointer
class FftwPlan : public QSharedData
{
public:
    FftwPlan (fftw_plan plan) :
        plan (plan)
    {
    }

    ~FftwPlan()
    {
        fftw_destroy_plan (plan);
    }

    fftw_plan plan;

private:
    FftwPlan (const FftwPlan &other);
    FftwPlan &operator= (const FftwPlan &other);
};

} // namespace

class FirFilterPrivate : public QSharedData
{
public:
    static QList<QVector<double> > load (QIODevice *device, unsigned limit);
    static quint32 nextPowerOfTwo (quint32 n);

    bool synchronous;
    QVector<double> taps;
    unsigned length;
    unsigned position;
    QVector<Complex> fftTaps;

    QExplicitlySharedDataPointer<FftwPlan> fftPlan;
    QExplicitlySharedDataPointer<FftwPlan> ifftPlan;
    FftwData<double> delay;
    FftwData<Complex> buffer;
    FftwData<double> output;
};

// FirFilterPrivate ============================================================

QList<QVector<double> > FirFilterPrivate::load (QIODevice *device,
        unsigned limit)
{
    QList<QVector<double> > result;

    double doubleChannels;
    if (device->read (reinterpret_cast<char*> (&doubleChannels), 8) != 8)
        throw SylError ("Unable to read number of channels");
    if (doubleChannels < 1)
        throw SylError ("Invalid number of channels");
    unsigned channels = qRound (doubleChannels);

    double doubleLength;
    if (device->read (reinterpret_cast<char*> (&doubleLength), 8) != 8)
        throw SylError ("Unable to read filter length");
    if (doubleLength < 1 || doubleLength >= INT_MAX / 8)
        throw SylError ("Invalid number of samples");
    unsigned length = qRound (doubleLength);

    for (unsigned i = 0; i < channels; ++i) {
        QVector<double> taps (length);
        if (device->read (reinterpret_cast<char*> (taps.data()), 8 * length) !=
                8 * length)
            throw SylError ("Unable to read filter coefficients");
        if (limit)
            taps.resize (qMin (unsigned (taps.size()), limit));
        result.append (taps);
    }

    return result;
}

quint32 FirFilterPrivate::nextPowerOfTwo (quint32 n)
{
    n = n - 1;
    n = n | (n >> 1);
    n = n | (n >> 2);
    n = n | (n >> 4);
    n = n | (n >> 8);
    n = n | (n >> 16);
    n = n + 1;
    return n;
}

// FirFilter ===================================================================

FirFilter::FirFilter (const QVector<double> &taps, bool synchronous) :
    d (new FirFilterPrivate)
{
    Q_ASSERT (taps.size());

    d->synchronous = synchronous;
    d->taps = taps;
    d->length = d->nextPowerOfTwo (taps.size());
    d->position = 0;
    d->taps.resize (2 * d->length);

    d->fftTaps.resize (d->length + 1);
    fftw_plan plan = fftw_plan_dft_r2c_1d (2 * d->length, d->taps.data(),
            reinterpret_cast<fftw_complex*> (d->fftTaps.data()),
            FFTW_ESTIMATE);
    fftw_execute (plan);
    fftw_destroy_plan (plan);

    const double scale = 1.0 / (2 * d->length);
    for (unsigned i = 0; i < d->length + 1; ++i)
        d->fftTaps[i] *= scale;

    d->delay = FftwData<double> (2 * d->length);
    d->buffer = FftwData<Complex> (d->length + 1);
    d->output = FftwData<double> (2 * d->length);
    // The given buffers are not actually used, because copy-on-write may mess
    // with the pointers; the guru plan execution is used instead
    d->fftPlan = new FftwPlan (fftw_plan_dft_r2c_1d (2 * d->length,
                d->delay.data(),
                reinterpret_cast<fftw_complex*> (d->buffer.data()),
                FFTW_ESTIMATE));
    d->ifftPlan = new FftwPlan (fftw_plan_dft_c2r_1d (2 * d->length,
                reinterpret_cast<fftw_complex*> (d->buffer.data()),
                d->output.data(),
                FFTW_ESTIMATE));
}

FirFilter::~FirFilter()
{
}

FirFilter::FirFilter (const FirFilter &other) :
    d (other.d)
{
}

FirFilter &FirFilter::operator= (const FirFilter &other)
{
    d = other.d;
    return *this;
}

unsigned FirFilter::length() const
{
    return d->length;
}

unsigned FirFilter::position() const
{
    return d->position;
}

QVector<double> FirFilter::taps() const
{
    return d->taps;
}

void FirFilter::process (double *data, unsigned dataLength)
{
    if (!dataLength)
        return;

    unsigned &position = d->position;
    const Complex * const fftTaps = d->fftTaps.data();
    double * const delay = d->delay.data();
    Complex * const buffer = d->buffer.data();
    double * const output = d->output.data();
    fftw_plan fftPlan = d->fftPlan->plan;
    fftw_plan ifftPlan = d->ifftPlan->plan;
    const unsigned length = d->length;
    const bool synchronous = d->synchronous;

    Q_ASSERT (!synchronous || dataLength % length == 0);

    unsigned pos = 0;
    while (dataLength) {
        const unsigned fillNeeded = length - (position % length);
        const unsigned fillAvailable = qMin (dataLength, fillNeeded);

        memcpy (delay + 2 * length - fillNeeded, data + pos,
                fillAvailable * sizeof (double));
        if (!synchronous)
            memcpy (data + pos, output + 2 * length - fillNeeded,
                    fillAvailable * sizeof (double));
        dataLength -= fillAvailable;
        pos += fillAvailable;
        position += fillAvailable;

        if (position % length == 0) {
            fftw_execute_dft_r2c (fftPlan,
                    delay, reinterpret_cast<fftw_complex*> (buffer));
            for (unsigned i = 0; i < length + 1; ++i)
                buffer[i] *= fftTaps[i];
            fftw_execute_dft_c2r (ifftPlan,
                    reinterpret_cast<fftw_complex*> (buffer), output);
            memcpy (delay, delay + length, length * sizeof (double));
        }

        if (synchronous)
            memcpy (data + pos - length, output + length,
                    length * sizeof (double));
    }
}

QList<QVector<double> > FirFilter::load (const QString &filePath,
        unsigned maximumLength)
{
    QFile file (filePath);
    if (!file.open (QFile::ReadOnly))
       throw SylError (QString ("Unable to read FIR coefficients from '%1': %2")
                .arg (file.fileName(), file.errorString()));
    return FirFilterPrivate::load (&file, maximumLength);
}

QList<QVector<double> > FirFilter::load (QIODevice *device,
        unsigned maximumLength)
{
    if (!device->open (QFile::ReadOnly))
        throw SylError (QString ("Unable to read FIR coefficients: %1")
                .arg (device->errorString()));
    return FirFilterPrivate::load (device, maximumLength);
}

} // namespace syl
