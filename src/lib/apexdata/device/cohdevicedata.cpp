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

#include "map/apexmap.h"

#include "cohdevicedata.h"

namespace apex
{

namespace data
{

CohDeviceData::CohDeviceData():
    DeviceData(TYPE_COH)
{
    setNumberOfChannels(1);

    setValueByType("powerup_count", 4000);
}

CohDeviceData::~CohDeviceData()
{
}

void CohDeviceData::setMap(ApexMap* map)
{
    apexMap.reset(map);
}

ApexMap* CohDeviceData::map() const
{
    return apexMap.data();
}

QString CohDeviceData::device() const
{
    return valueByType("device").toString();
}

int CohDeviceData::triggerType() const
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

int CohDeviceData::powerupCount() const
{
    return valueByType("powerup_count").toInt();
}

float CohDeviceData::volume() const
{
    return valueByType("volume").toDouble();
}

}// ns data

}// ns apex
