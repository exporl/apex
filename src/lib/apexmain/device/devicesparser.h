/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#ifndef _EXPORL_SRC_LIB_APEXMAIN_DEVICE_DEVICESPARSER_H_
#define _EXPORL_SRC_LIB_APEXMAIN_DEVICE_DEVICESPARSER_H_

#include "apexdata/device/devicedata.h"
#include "apexdata/device/devicesdata.h"

#include "apextools/apextypedefs.h"

#include <QObject>

namespace apex
{

namespace data
{
class PluginControllerData;
class ParameterManagerData;
}

namespace parser
{

struct tAllDevices {
    data::DevicesData outputdevices;
    data::DevicesData controldevices;
};

class APEX_EXPORT DevicesParser
{
    Q_DECLARE_TR_FUNCTIONS(DevicesParser);

public:
    DevicesParser();
    ~DevicesParser();

    tAllDevices Parse(const QDomElement &dom, data::ParameterManagerData *pm);

private:
    data::DeviceData *ParseDevice(const QDomElement &p_base,
                                  data::ParameterManagerData *pm);
};
}
}

#endif
