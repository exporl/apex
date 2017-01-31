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
#include "arfilter.h"

#include <QFile>
#include <QString>
#include <QVector>

namespace syl
{

class ArFilterPrivate : public QSharedData
{
public:
    static QVector<double> load (QIODevice *device, unsigned limit);

    QVector<double> taps;
    unsigned length;
    unsigned position;
    QVector<double> delay;
};

// ArFilterPrivate =============================================================

QVector<double> ArFilterPrivate::load (QIODevice *device, unsigned limit)
{
    QVector<double> result;

    double doubleChannels;
    if (device->read (reinterpret_cast<char*> (&doubleChannels), 8) != 8 ||
            doubleChannels != 2.0)
        throw SylError ("Unable to read number of channels");

    double doubleLength;
    if (device->read (reinterpret_cast<char*> (&doubleLength), 8) != 8)
        throw SylError ("Unable to read filter length");
    unsigned length = qBound (1u, unsigned (doubleLength), 64 * 1024u);

    result.resize (length);

    if (device->read (reinterpret_cast<char*> (result.data()), 8 * length) !=
            8 * length)
        throw SylError ("Unable to read filter coefficients");
    if (result[0] != 1.0)
        throw SylError ("Invalid AR filter coefficients (b0!=1.0)");
    for (unsigned i = 1; i < length; ++i)
        if (result[i] != 0.0)
            throw SylError ("Invalid AR filter coefficients (bi!=0.0|i>0)");

    if (device->read (reinterpret_cast<char*> (result.data()), 8 * length) !=
            8 * length)
        throw SylError ("Unable to read filter coefficients");
    if (result[0] != 1.0)
        throw SylError ("Invalid AR filter coefficients (a0!=1.0)");

    if (limit)
        result.resize (qMin (unsigned (result.size()), limit));

    return result;
}

// ArFilter ====================================================================

ArFilter::ArFilter (const QVector<double> &taps) :
    d (new ArFilterPrivate)
{
    Q_ASSERT (taps.size());
    Q_ASSERT (taps[0] == 1.0);

    d->taps = taps;
    d->length = taps.size();
    d->position = 0;
    d->delay.resize (d->length);
}

ArFilter::~ArFilter()
{
}

ArFilter::ArFilter (const ArFilter &other) :
    d (other.d)
{
}

ArFilter &ArFilter::operator= (const ArFilter &other)
{
    d = other.d;
    return *this;
}

unsigned ArFilter::length() const
{
    return d->length;
}

unsigned ArFilter::position() const
{
    return d->position;
}

QVector<double> ArFilter::taps() const
{
    return d->taps;
}

void ArFilter::process (double *data, unsigned dataLength)
{
    if (!dataLength)
        return;

    double * const delay = d->delay.data();
    const double * const taps = d->taps.data();
    const unsigned length = d->length;

    for (unsigned i = 0; i < dataLength; ++i) {
        for (unsigned j = length - 1; j > 0; --j)
            delay[j] = delay[j - 1];

        for (unsigned j = 1; j < length; ++j)
            data[i] -= delay[j] * taps[j];

        delay[0] = data[i];
    }

    d->position += dataLength;
}

QVector<double> ArFilter::load (const QString &filePath, unsigned maximumLength)
{
    QFile file (filePath);
    if (!file.open (QFile::ReadOnly))
       throw SylError (QString ("Unable to read AR coefficients from '%1': %2")
                .arg (file.fileName(), file.errorString()));
    return ArFilterPrivate::load (&file, maximumLength);
}

QVector<double> ArFilter::load (QIODevice *device, unsigned maximumLength)
{
    if (!device->open (QFile::ReadOnly))
        throw SylError (QString ("Unable to read AR coefficients: %1")
                .arg (device->errorString()));
    return ArFilterPrivate::load (device, maximumLength);
}

} // namespace syl
