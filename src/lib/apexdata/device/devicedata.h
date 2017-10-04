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

#ifndef _EXPORL_SRC_LIB_APEXDATA_DEVICE_DEVICEDATA_H_
#define _EXPORL_SRC_LIB_APEXDATA_DEVICE_DEVICEDATA_H_

#include "../parameters/simpleparameters.h"

namespace apex
{

namespace data
{

enum DeviceType {
    TYPE_NONE = -1,
    TYPE_WAVDEVICE,
    TYPE_COH,
    TYPE_DUMMY,
    TYPE_PLUGINCONTROLLER
};

/**
 * Base class for the data classes for each device
 *
 * @author Tom Francart,,, <tom.francart@med.kuleuven.be>
 */
class APEXDATA_EXPORT DeviceData : public SimpleParameters
{
public:
    virtual ~DeviceData();

    // getters

    DeviceType deviceType() const;
    virtual bool isControlDevice() const;
    virtual unsigned numberOfChannels() const;

    // setters

    virtual void setNumberOfChannels(unsigned n);

    bool operator==(const DeviceData &other) const;
    void setSilent(bool newSilent = true);
    bool isSilent() const;

protected:
    DeviceData(DeviceType type);

    bool isControlDev;

private:
    DeviceType devType;
    bool silent;
};

} // ns data

} // ns apex

#endif
