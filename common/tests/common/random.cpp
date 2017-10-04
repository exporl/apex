/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 3 of the License, or          *
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

#include "common/random.h"
#include "common/testutils.h"

#include "commontest.h"

#include <QTest>

using namespace cmn;

void CommonTest::random()
{
    TEST_EXCEPTIONS_TRY

    { // Unsigned ints
        Random rand(122760);
        QCOMPARE(rand.nextUInt(), 2770862625u);
        QCOMPARE(rand.nextULongLong(), Q_UINT64_C(2785759620113032781));
        QCOMPARE(rand.nextDouble(), 0.8173322904425151);
        QCOMPARE(rand.nextFloat(), 0.8239248f);
        char b[1];
        rand.nextBytes(b, 0);
        QCOMPARE(rand.nextUInt(), 3395488870u);
    }
    { // Signed ints
        Random rand(122760);
        QCOMPARE(rand.nextInt(), -1524104671);
        rand.nextLongLong();
        rand.nextDouble();
        rand.nextFloat();
        char b[1];
        rand.nextBytes(b, 0);
        QCOMPARE(rand.nextInt(), -899478426);
    }
    { // doubles with minimum and maxmimum
        Random rand(122760);
        rand.nextInt();
        rand.nextLongLong();
        QCOMPARE(rand.nextDouble(-2, 3), 2.0866614522125753);
    }
    { // Less bits than provided by one random value
        Random rand(122760);
        rand.nextInt();
        rand.nextLongLong();
        rand.nextDouble();
        rand.nextFloat();
        char b[3];
        rand.nextBytes(b, 3);
        QCOMPARE(b[0], char(102));
        QCOMPARE(b[1], char(12));
        QCOMPARE(b[2], char(99));
        QCOMPARE(rand.nextInt(), -1550323395);
    }
    { // Same number of bits as provided by one random value
        Random rand(122760);
        rand.nextInt();
        rand.nextLongLong();
        rand.nextDouble();
        rand.nextFloat();
        char b[4];
        rand.nextBytes(b, 4);
        QCOMPARE(b[0], char(102));
        QCOMPARE(b[1], char(12));
        QCOMPARE(b[2], char(99));
        QCOMPARE(b[3], char(-54));
        QCOMPARE(rand.nextInt(), -1550323395);
    }
    { // More bits than provided by one random value
        Random rand(122760);
        rand.nextInt();
        rand.nextLongLong();
        rand.nextDouble();
        rand.nextFloat();
        char b[5];
        rand.nextBytes(b, 5);
        QCOMPARE(b[0], char(102));
        QCOMPARE(b[1], char(12));
        QCOMPARE(b[2], char(99));
        QCOMPARE(b[3], char(-54));
        QCOMPARE(b[4], char(61));
        QCOMPARE(rand.nextInt(), -270809961);
    }
    { // power of 2
        Random rand(122760);
        QCOMPARE(rand.nextUInt(0x1000), 0xa52u);
        QCOMPARE(rand.nextUInt(), 648610205u);
    }
    { // triggers code to prevent uneven distributions
        Random rand(122760);
        QCOMPARE(rand.nextUInt((1 << 31) + 1), 648610205u);
        QCOMPARE(rand.nextUInt(), 1786177101u);
    }

    TEST_EXCEPTIONS_CATCH
}
