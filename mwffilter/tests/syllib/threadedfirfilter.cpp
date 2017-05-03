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

#include "common/testutils.h"

#include "syllib/threadedfirfilter.h"

#include "tester.h"

using namespace syl;

namespace
{

class FilterTest {
public:
    FilterTest (const ThreadedFirFilter *filter);
    void processTime (double *data, unsigned dataLength);

private:
    const ThreadedFirFilter * const filter;
    QVector<double> delay;
};

FilterTest::FilterTest (const ThreadedFirFilter *filter) :
    filter (filter)
{
    delay.resize (filter->length());
}

void FilterTest::processTime (double *data, unsigned dataLength)
{
    if (!dataLength)
        return;

    double * const delay = this->delay.data();
    const QVector<double> tapsVector = filter->taps();
    const double * const taps = tapsVector.data();
    const unsigned length = filter->length();

    for (unsigned i = 0; i < dataLength; ++i) {
        for (unsigned j = 0; j < length - 1; ++j)
            delay[j] = delay[j + 1];
        delay[length - 1] = data[i];

        double result = 0;
        for (unsigned j = 0; j < length; ++j)
            result += delay[j] * taps[length - 1 - j];

        data[i] = result;
    }
}

} // namespace

void TestSyl::threadedFirFilter()
{
    TEST_EXCEPTIONS_TRY

    const unsigned char dump[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x40,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x40,
        0xa4, 0xd1, 0xc5, 0xe4, 0x2d, 0xd1, 0x51, 0x3f,
        0xac, 0x3d, 0xfa, 0xe1, 0x09, 0xcc, 0x51, 0xbf,
        0x6d, 0x16, 0x76, 0x23, 0x23, 0x20, 0x52, 0x3f,
        0xc6, 0xad, 0x60, 0x61, 0x45, 0x0d, 0x53, 0x3f,
        0x93, 0xd9, 0x1e, 0x39, 0xfc, 0x84, 0x52, 0xbf,
        0xc4, 0xc5, 0x5e, 0xb4, 0x44, 0x51, 0x53, 0x3f,
        0x5d, 0x7f, 0xef, 0x64, 0x58, 0x5b, 0x55, 0x3f,
        0xf9, 0xe7, 0x1b, 0xb9, 0x3d, 0xcf, 0x56, 0xbf,
        0x05, 0x28, 0x3b, 0x92, 0xc3, 0xf7, 0x54, 0x3f,
        0x87, 0x3f, 0xcc, 0x24, 0x5c, 0xc0, 0x55, 0x3f,
        0x4b, 0x4a, 0xeb, 0x4b, 0x26, 0xd7, 0x56, 0xbf,
        0xf9, 0x8b, 0x66, 0xaa, 0x24, 0xf7, 0x57, 0x3f};

    const double taps[] = {
        1.0874698810684280e-03, -1.0862442503912590e-03,
        1.1062949323287455e-03,  0,
        1.1628320257938015e-03, -1.1303389660334048e-03,
        1.1790438597752842e-03,  0,
        1.3035166955361534e-03, -1.3921835674737244e-03,
        1.2797746539257037e-03,  0,
        1.3276004639841425e-03, -1.3940690984567568e-03,
        1.4627321303144465e-03,  0};

    QByteArray array (reinterpret_cast<const char*> (dump), sizeof (dump));

    { // Basic loading, rounding up to 4 taps
        QObject owner;
        QBuffer buffer (&array);
        QList<ThreadedFirFilter*> filters;
        Q_FOREACH (const QVector<double> &taps,
                FirFilter::load (&buffer))
            filters.append (new ThreadedFirFilter (taps, false, &owner));
        QCOMPARE (filters.size(), 4);
        QCOMPARE (filters[0]->length(), 4u);
        QCOMPARE (filters[1]->length(), 4u);
        QCOMPARE (filters[2]->length(), 4u);
        QCOMPARE (filters[3]->length(), 4u);
        ARRAYCOMP (filters[0]->taps(), taps +  0, 4);
        ARRAYCOMP (filters[1]->taps(), taps +  4, 4);
        ARRAYCOMP (filters[2]->taps(), taps +  8, 4);
        ARRAYCOMP (filters[3]->taps(), taps + 12, 4);

        double data[20];
        memset (data, 0, sizeof (data));
        data[0] = 1.0;
        const ThreadedFirFilter * const filter = filters.at (0);
        FilterTest test (filter);
        test.processTime (data, 20);
        ARRAYCOMP (data, taps + 0, 4);
    }

    { // Now with FFT
        QObject owner;
        QBuffer buffer (&array);
        QList<ThreadedFirFilter*> filters;
        Q_FOREACH (const QVector<double> &taps,
                FirFilter::load (&buffer))
            filters.append (new ThreadedFirFilter (taps, false, &owner));
        ThreadedFirFilter * const filter = filters.at (0);

        double data[20];
        memset (data, 0, sizeof (data));
        data[0] = 1.0;
        QCOMPARE (filter->position(), 0u);
        filter->process (data, 20, NULL);
        QCOMPARE (filter->position(), 20u);
        ARRAYFUZZCOMP (data + 4, taps + 0, 1e-18, 4);
    }

    { // Basic loading, limited to 2
        QObject owner;
        QBuffer buffer (&array);
        QList<ThreadedFirFilter*> filters;
        Q_FOREACH (const QVector<double> &taps,
                FirFilter::load (&buffer, 2))
            filters.append (new ThreadedFirFilter (taps, false, &owner));
        QCOMPARE (filters.size(), 4);
        QCOMPARE (filters[0]->length(), 2u);
        QCOMPARE (filters[1]->length(), 2u);
        QCOMPARE (filters[2]->length(), 2u);
        QCOMPARE (filters[3]->length(), 2u);
        QCOMPARE (filters[0]->taps()[2], 0.0);
        QCOMPARE (filters[0]->taps()[3], 0.0);
        QCOMPARE (filters[1]->taps()[2], 0.0);
        QCOMPARE (filters[1]->taps()[3], 0.0);
        QCOMPARE (filters[2]->taps()[2], 0.0);
        QCOMPARE (filters[2]->taps()[3], 0.0);
        QCOMPARE (filters[3]->taps()[2], 0.0);
        QCOMPARE (filters[3]->taps()[3], 0.0);
    }

    { // Filter position
        QObject owner;
        QBuffer buffer (&array);
        QList<ThreadedFirFilter*> filters;
        Q_FOREACH (const QVector<double> &taps,
                FirFilter::load (&buffer))
            filters.append (new ThreadedFirFilter (taps, false, &owner));
        ThreadedFirFilter * const filter = filters[0];
        double data[128];
        QCOMPARE (filter->position(), 0u);
        QCOMPARE (filters[0]->position(), 0u);
        QCOMPARE (filters[1]->position(), 0u);
        filters[0]->process (data, 128, NULL);
        QCOMPARE (filters[0]->position(), 128u);
        QCOMPARE (filter->position(), 128u);
        QCOMPARE (filters[1]->position(), 0u);
    }

    // Processing
    double data[128];
    for (unsigned i = 0; i < 128; ++i)
        data[i] = rand() / (RAND_MAX + 1.0);
    double data2[128];
    double data3[128];
    memcpy (data2, data, sizeof (data));
    memcpy (data3, data, sizeof (data));

    {
        QObject owner;
        QBuffer buffer (&array);
        QList<ThreadedFirFilter*> filters;
        Q_FOREACH (const QVector<double> &taps,
                FirFilter::load (&buffer))
            filters.append (new ThreadedFirFilter (taps, false, &owner));
        const ThreadedFirFilter * const filter = filters.at (0);
        FilterTest test (filter);
        test.processTime (data, 128);
    }

    { // Semaphore
        QObject owner;
        QSemaphore semaphore;
        QBuffer buffer (&array);
        QList<ThreadedFirFilter*> filters;
        Q_FOREACH (const QVector<double> &taps, FirFilter::load (&buffer))
            filters.append (new ThreadedFirFilter (taps, false, &owner));
        ThreadedFirFilter * const filter = filters.at (0);
        int count = 0;
        for (unsigned i = 0; i < 128; ++count) {
            const unsigned length = qMin (128u, i + (rand() % 15)) - i;
            filter->process (data2 + i, length, &semaphore);
            i += length;
        }
        QCOMPARE (semaphore.available(), count);
    }
    ARRAYFUZZCOMP (data, data2 + 4, 1e-18, 128 - 4);

    { // synchronous
        QObject owner;
        QSemaphore semaphore;
        QBuffer buffer (&array);
        QList<ThreadedFirFilter*> filters;
        Q_FOREACH (const QVector<double> &taps, FirFilter::load (&buffer))
            filters.append (new ThreadedFirFilter (taps, true, &owner));
        ThreadedFirFilter * const filter = filters.at (0);
        int count = 0;
        for (unsigned i = 0; i < 128; ++count) {
            const unsigned length = filter->length();
            filter->process (data3 + i, length, &semaphore);
            i += length;
        }
        QCOMPARE (semaphore.available(), count);
    }
    ARRAYFUZZCOMP (data, data3, 1e-18, 128);

    TEST_EXCEPTIONS_CATCH
}
