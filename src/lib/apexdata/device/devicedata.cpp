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

#include "devicedata.h"

namespace apex
{

namespace data
{

DeviceData::DeviceData(DeviceType type) : isControlDev(false), devType(type), silent(false)
{
}


DeviceData::~DeviceData()
{
}

bool DeviceData::operator==(const DeviceData& other) const
{
    return  SimpleParameters::operator==(other) &&
            id() == other.id() &&
            devType == other.devType &&
            isControlDev == other.isControlDev;
}
DeviceType DeviceData::deviceType() const
{
    return devType;
}

bool DeviceData::isControlDevice() const
{
    return isControlDev;
}

unsigned int DeviceData::numberOfChannels() const
{
    return valueByType("channels").toUInt();
}
void DeviceData::setNumberOfChannels(unsigned int n)
{
    setValueByType("channels", n);
}

void DeviceData::setSilent(bool newSilent) {
    silent = newSilent;
}

bool DeviceData::isSilent() const {
    return silent;
}
}

}
