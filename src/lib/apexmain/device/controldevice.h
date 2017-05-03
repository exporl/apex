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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_DEVICE_CONTROLDEVICE_H_
#define _EXPORL_SRC_LIB_APEXMAIN_DEVICE_CONTROLDEVICE_H_

#include "iapexdevice.h"

#include <QObject>

namespace apex {

namespace device {

/**
Interfaces for devices that do not output samples but rather control stuff, like mixers or external attenuators

        @author Tom Francart,,, <tom.francart@med.kuleuven.be>
*/
class ControlDevice : public QObject,
                      public IApexDevice
{
Q_OBJECT
public:
    ControlDevice( data::DeviceData* a_pParameters );

    ~ControlDevice();

public Q_SLOTS:
    /*
     *  Can be used for synchronizing ControlDevice output with OutputDevice output
     *  Currently used by oscController plugin- signal stimulusStarted is emitted from OutputDevice (wavdevice)
     *  This slot is called inside plugincontroller (inherits from this class) which then calls the plugin's playStimulus method
     *  Connection happens in experimentcontrol.cpp @ loadExperiment()
     */
    virtual void syncControlDeviceOutput()    {    }

};

}

}

#endif
