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

#include "wavdevicedata.h"

namespace apex
{

namespace data
{

// WavDeviceData ===============================================================

WavDeviceData::WavDeviceData() : DeviceData(TYPE_WAVDEVICE)
{
    setValueByType("driver", "portaudio");
    setValueByType("card", "default");
    setValueByType("channels", 2);
    setValueByType("samplerate", 44100);
    setValueByType("buffersize", -1);
//    setValueByType("defaultbuffersize", 8192);
    setValueByType("blocksize", 8192);      // Note: if set to -1, the sound card buffer size will be used by default
    setValueByType("buffersize_apex", 1);
    setValueByType("padzero", 0);
    setValueByType("setcardmixer", false);
}

WavDeviceData::~WavDeviceData()
{
}

QString WavDeviceData::driverString() const
{
    return valueByType ("driver").toString();
}

void WavDeviceData::setDriverString(const QString& driver)
{
    setValueByType("driver", driver);
}

double WavDeviceData::internalBufferSize() const
{
    return valueByType ("buffersize_apex").toDouble();
}

void WavDeviceData::setInternalBufferSize (double bs)
{
    setValueByType ("buffersize_apex", bs);
}

int WavDeviceData::blockSize() const
{
    return valueByType ("blocksize").toInt();
}

void WavDeviceData::setBlockSize(unsigned bs)
{
    setValueByType ("blocksize", bs);
}

int WavDeviceData::bufferSize() const
{
    return valueByType ("buffersize").toInt();
}

/*unsigned WavDeviceData::defaultBufferSize() const
{
    return valueByType ("defaultbuffersize").toUInt();
}*/

void WavDeviceData::setBufferSize(int bs)
{
    setValueByType ("buffersize", bs);
}

unsigned long WavDeviceData::sampleRate() const
{
    return valueByType ("samplerate").toUInt();
}

void WavDeviceData::setSampleRate (unsigned long fs)
{
    setValueByType ("samplerate", unsigned (fs));
}

QString WavDeviceData::cardName() const
{
    return valueByType("card").toString();
}

}//ns data

}//ns apex
