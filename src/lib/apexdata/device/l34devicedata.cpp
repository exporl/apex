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

#include "l34devicedata.h"

namespace apex
{

namespace data
{

L34DeviceData::L34DeviceData(): DeviceData(TYPE_L34), apexMap(0)
{
    setNumberOfChannels(1);

    setValueByType("device_type", "L34");
    setValueByType("device_id", 1);
    setValueByType("implant", "cic3");
    setValueByType("trigger", "none");
    setValueByType("powerup_count", 4000);
}

L34DeviceData::~L34DeviceData()
{
}

void L34DeviceData::setMap(ApexMap* map)
{
    Q_ASSERT(map);
    apexMap = map;
}

ApexMap* L34DeviceData::map() const
{
    Q_ASSERT(apexMap);
    return apexMap;
}

int L34DeviceData::deviceId() const
{
    return valueByType("device_id").toInt();
}

const QString L34DeviceData::deviceType() const
{
    return valueByType("device_type").toString();
}

int L34DeviceData::triggerType() const
{
    QString triggertype = valueByType("trigger").toString();

    if (triggertype.isEmpty() || triggertype=="none")
        return TRIGGER_NONE;
    if (triggertype=="in")
        return TRIGGER_IN;
    if (triggertype=="out")
        return TRIGGER_OUT;

    qFatal("invalid trigger type");
    return -1;
}

int L34DeviceData::powerupCount() const
{
    return valueByType("powerup_count").toInt();
}

float L34DeviceData::volume() const
{
    return valueByType("volume").toDouble();
}

const QString L34DeviceData::implantType() const
{
    return valueByType("implant").toString();
}

}// ns data

}// ns apex
