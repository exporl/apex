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

#ifndef APEX_DATADEVICESDATA_H
#define APEX_DATADEVICESDATA_H

#include "devicedata.h"

//from libtools
#include "apextypedefs.h"

namespace apex
{

namespace data
{

/**
 * @author Tom Francart,,, <tom.francart@med.kuleuven.be>
 *
 * DevicesData does not take ownership of DeviceData objects, they are
 * destructed in ExperimentData
 */
class APEXDATA_EXPORT DevicesData : public DeviceDataMap
{
    public:

        DevicesData();
        ~DevicesData();

        //getters

        const QString& masterDevice() const;
        DeviceData* deviceData(const QString& name) const;

        //setters

        void setMasterDevice(const QString& device);

        bool operator==(const DevicesData& other) const;

    private:

        QString masterDev;

};

}

}

#endif
