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

#include "apexdata/device/wavdevicedata.h"

#include "wavdeviceparser.h"

namespace apex
{

namespace parser
{

WavDeviceParser::WavDeviceParser()
{
}

WavDeviceParser::~WavDeviceParser()
{
}

void WavDeviceParser::Parse(const QDomElement &base, data::WavDeviceData *p)
{
    SimpleParametersParser::Parse(base, p);
}

void WavDeviceParser::AddParameter(data::SimpleParameters *p,
                                   const QDomElement &e, const QString &owner,
                                   const QString &type, const QString &id,
                                   const QVariant &value, const int channel)
{
    SimpleParametersParser::AddParameter(p, e, owner, type, id, value, channel);

    data::WavDeviceData *q = dynamic_cast<data::WavDeviceData *>(p);
    Q_ASSERT(q);

    /*if (type=="driver") {
        q->SetDriver( stimulus::fSndStringToEnum(value) );
    } else */ if (type == "samplerate") {
        q->setSampleRate(value.toULongLong());
    } else if (type == "buffersize") {
        qCDebug(APEX_RS, "Setting buffer size to %d", value.toUInt());
        q->setBufferSize(value.toUInt());
    } else if (type == "blocksize") {
        qCDebug(APEX_RS, "Setting internal stream block size to %d",
                value.toUInt());
        q->setBlockSize(value.toUInt());
    }
}
}
}
