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

#ifndef _EXPORL_SRC_LIB_APEXDATA_DEVICE_COHDEVICEDATA_H_
#define _EXPORL_SRC_LIB_APEXDATA_DEVICE_COHDEVICEDATA_H_

#include "devicedata.h"

// from libtools
#include "apextools/global.h"

namespace apex
{

namespace data
{

enum { TRIGGER_NONE, TRIGGER_IN, TRIGGER_OUT };

class ApexMap;

/**
 * @author Tom Francart,,,
 */
class APEXDATA_EXPORT CohDeviceData : public DeviceData
{
public:
    CohDeviceData();
    ~CohDeviceData();

    ApexMap *map() const;
    QString device() const;
    int triggerType() const;
    float volume() const;
    int powerupCount() const;

    // takes ownership
    void setMap(ApexMap *map);

private:
    QScopedPointer<ApexMap> apexMap;
};
}
}

#endif
