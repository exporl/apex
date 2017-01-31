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

#ifndef APEX_DATAWAVDEVICEDATA_H
#define APEX_DATAWAVDEVICEDATA_H

#include "device/devicedata.h"

//from libtools
#include "global.h"

namespace apex
{

namespace data
{

class APEXDATA_EXPORT WavDeviceData : public DeviceData
{
    public:

        WavDeviceData();
        virtual ~WavDeviceData();

        //getters

        QString driverString() const;
        double internalBufferSize() const;
        int bufferSize() const;
        //unsigned defaultBufferSize() const;
        unsigned blockSize() const;
        unsigned long sampleRate() const;
        QString cardName() const;

        //setters

        void setDriverString(const QString& driver);
        void setInternalBufferSize(double bs);
        void setBufferSize(int bs);
        void setBlockSize(unsigned bs);
        void setSampleRate(unsigned long fs);
};

}

}

#endif
