/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 3.                                               *
 *                                                                            *
 * APEX 3 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 3 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 3.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "qicparser.h"

namespace apex
{

namespace stimulus
{

QicParser::QicParser(const QByteArray &data) :
    data(data)
{
}

QicParser::~QicParser()
{
}

L34Data QicParser::GetData()
{
    L34Data buffer(data.size() / 6);

    qint16 tChannel = 0;
    float tMagnitude = -1;

    for (unsigned i = 0; i < unsigned(buffer.size()); ++i) {
        tChannel = *reinterpret_cast<const qint16*>(data.data() + i * 6);
        tMagnitude = *reinterpret_cast<const float*>(data.data() + i * 6 + 2);

        if (tChannel <= 0 || tMagnitude < 0) {           // powerup
            tChannel = 0;
            tMagnitude = 0;
        }

        buffer[i] = L34Stimulus(tChannel, tMagnitude);
    }

//    qDebug("Counted %u pulses in qic file", buffer.size());
    return buffer;
}

L34Data QicParser::GetExtendedData()
{
    L34Data buffer(data.size() / 20);

    qint16 tChannel = 0;
    float tMagnitude = -1;
    float tPeriod = -1;
    quint16 tMode = 0;
    float tPhaseWidth = -1;
    float tPhaseGap = -1;

    for (unsigned i = 0; i < unsigned(buffer.size()); ++i) {
        tChannel = *reinterpret_cast<const qint16*>(data.data() + i * 20);
        tMagnitude = *reinterpret_cast<const float*>(data.data() + i * 20 + 2);
        tPeriod = *reinterpret_cast<const float*>(data.data() + i * 20 + 6);
        tMode = *reinterpret_cast<const quint16*>(data.data() + i * 20 + 10);
        tPhaseWidth = *reinterpret_cast<const float*>(data.data() + i * 20 + 12);
        tPhaseGap = *reinterpret_cast<const float*>(data.data() + i * 20 + 16);

        if (tMagnitude == -1) {           // powerup
            tChannel = 0;
            tMagnitude = 0;
        }

        buffer[i] = L34Stimulus(tChannel, tMagnitude, tPeriod, tMode, tPhaseWidth, tPhaseGap);
    }

//    qDebug("Counted %u pulses in qicext file", buffer.size());
    return buffer;
}

}

}
