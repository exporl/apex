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

#include "apexdata/device/plugincontrollerdata.h"

#include "apextools/apexpluginloader.h"

#include "plugincontroller.h"
#include "plugincontrollerinterface.h"

namespace apex
{

namespace device
{

PluginController::PluginController(data::PluginControllerData *p_parameters)
    : ControlDevice(p_parameters), m_param(p_parameters)
{
    MakePlugin();
}

PluginController::~PluginController()
{
}

} // ns device

} // ns apex

void apex::device::PluginController::MakePlugin()
{
    PluginControllerCreator *creator =
        createPluginCreator<PluginControllerCreator>(m_param->plugin());
    m_plugin.reset(creator->createController(m_param->plugin()));
    return;
}

bool apex::device::PluginController::SetParameter(const QString &type,
                                                  const int channel,
                                                  const QVariant &value)
{
    Q_ASSERT(m_plugin);
    return m_plugin->setParameter(type, channel, value.toString());
}

void apex::device::PluginController::Reset()
{
    Q_ASSERT(m_plugin);
    m_plugin->resetParameters();
}

void apex::device::PluginController::Prepare()
{
    Q_ASSERT(m_plugin);
    m_plugin->prepare();
}

void apex::device::PluginController::release()
{
    Q_ASSERT(m_plugin);
    m_plugin->release();
}

void apex::device::PluginController::syncControlDeviceOutput()
{
    RETURN_IF_FAIL(m_plugin);
    m_plugin->playStimulus();
}
