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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_DEVICE_PLUGINCONTROLLER_H_
#define _EXPORL_SRC_LIB_APEXMAIN_DEVICE_PLUGINCONTROLLER_H_

#include "controldevice.h"

#include <QObject>

class PluginControllerInterface;

namespace apex
{

namespace data
{
class PluginControllerData;
}

namespace device
{

class PluginController : public ControlDevice
{
    Q_OBJECT
public:
    PluginController(data::PluginControllerData *a_pParameters);
    virtual ~PluginController();

    virtual bool SetParameter(const QString &type, const int channel,
                              const QVariant &value);

    virtual void Reset();
    virtual void Prepare();
    virtual void release();

    virtual void syncControlDeviceOutput();

private:
    void MakePlugin();

private:
    data::PluginControllerData *m_param;
    QScopedPointer<PluginControllerInterface> m_plugin;
};

} // ns device
} // ns apex

#endif
