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

#ifndef _SYL_SRC_TESTS_TESTER_H_
#define _SYL_SRC_TESTS_TESTER_H_

#include <QtTest>

#define FUZZCOMP(value, expected, delta)                                       \
    fuzzComp (value, expected, delta, #value, #expected, __FILE__, __LINE__)

template <typename A, typename B, typename C>
void fuzzComp (A value, B expected, C delta, const char *valueText,
        const char *expectedText, const char *file, int line)
{
    if (!(qAbs (value - expected) < delta))
        QTest::qFail (qPrintable (QString
                    ("Compared values differ more than %1.\n"
                     "  Actual (%4): '%2'\n"
                     "  Expected (%5): '%3'")
                    .arg (delta).arg (value).arg (expected)
                    .arg (valueText, expectedText)), file, line);
}

#define ARRAYCOMP(value, expected, size)                                       \
    arrayComp (value, expected, size, __FILE__, __LINE__)

template <typename A, typename B>
void arrayComp (A value, B expected, unsigned size, const char *file, int line)
{
    for (unsigned i = 0; i < size; ++i)
        if (value[i] != expected[i])
            QTest::qFail (qPrintable (QString
                    ("Compared values differ at %1.\n"
                     "  Actual: '%2'\n"
                     "  Expected: '%3'")
                    .arg (i).arg (value[i]).arg (expected[i])), file, line);
}

#define ARRAYFUZZCOMP(value, expected, delta, size)                            \
    arrayFuzzComp (value, expected, delta, size, __FILE__, __LINE__)

template <typename A, typename B, typename C>
void arrayFuzzComp (A value, B expected, C delta, unsigned size,
        const char *file, int line)
{
    for (unsigned i = 0; i < size; ++i)
        if (!(qAbs (value[i] - expected[i]) < delta))
            QTest::qFail (qPrintable (QString
                    ("Compared values differ more than %1 at %2.\n"
                     "  Actual: '%3'\n"
                     "  Expected: '%4'")
                    .arg (delta).arg (i).arg (value[i])
                    .arg (expected[i])), file, line);
}

class TestSyl: public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void fileSink();
    void fileSink_data();
    void arFilter();
    void firFilter();
    void threadedFirFilter();
    void hrtfMwf();
    void hrtfMwf_data();
    void hrtfMwfAdaptive();
    void hrtfMwfAdaptive_data();
    void vad();
};

#endif
