/******************************************************************************
 * Copyright (C) 2008 Michael Hofmann <mh21@piware.de>                        *
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

#include "testbench.h"
#include "stimulus/l34/qicparser.h"

using namespace apex::stimulus;

void Tests::testQic()
{
    const unsigned char data[] = {
        0x01, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x01, 0x00, 0x00, 0x00, 0x80, 0x3f,
        0x01, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x01, 0x00, 0x00, 0x00, 0x80, 0x3f,
        0x01, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x01, 0x00, 0x00, 0x00, 0x80, 0x3f,
        0x01, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x01, 0x00, 0x00, 0x00, 0x80, 0x3f,
        0x01, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x01, 0x00, 0x00, 0x00, 0x80, 0x3f,
        0x01, 0x00, 0x00, 0x00, 0x80, 0xbf, 0x01, 0x00, 0x00, 0x00, 0x80, 0xbf,
        0x01, 0x00, 0x00, 0x00, 0x80, 0xbf, 0x01, 0x00, 0x00, 0x00, 0x80, 0xbf};

    QicParser parser(QByteArray(reinterpret_cast<const char *>(data), sizeof(data)));
    L34Data parsedData(parser.GetData());
    QCOMPARE(parsedData.size(), 14u);
    QCOMPARE(parsedData[0].channel, qint16(1));
    QCOMPARE(parsedData[0].magnitude, 1.0f);
    QCOMPARE(parsedData[0].period, -1.0f);
    QCOMPARE(parsedData[0].mode, qint16(-1));
    QCOMPARE(parsedData[0].phaseWidth, -1.0f);
    QCOMPARE(parsedData[0].phaseGap, -1.0f);
    QCOMPARE(parsedData[10].channel, qint16(0));
    QCOMPARE(parsedData[10].magnitude, 0.0f);
    QCOMPARE(parsedData[10].period, -1.0f);
    QCOMPARE(parsedData[10].mode, qint16(-1));
    QCOMPARE(parsedData[10].phaseWidth, -1.0f);
    QCOMPARE(parsedData[10].phaseGap, -1.0f);
}
