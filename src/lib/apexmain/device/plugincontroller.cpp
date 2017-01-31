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
 
#include "plugincontroller.h"
#include "plugincontrollerinterface.h"

#include "services/mainconfigfileparser.h"      // paths
#include "services/pluginloader.h"

//from libdata
#include "device/plugincontrollerdata.h"

#include <QString>
#include <QPluginLoader>

namespace apex {

namespace device {

PluginController::PluginController( data::PluginControllerData* p_parameters ):
        ControlDevice(p_parameters),
        m_param(p_parameters),
        m_plugin(0)
{
    MakePlugin();


}





} // ns device


} // ns apex

void apex::device::PluginController::MakePlugin()
{
    PluginControllerCreator* creator = PluginLoader::Get().
            createPluginCreator<PluginControllerCreator>(m_param->plugin());
    m_plugin.reset (creator->createController (m_param->plugin()));
    return;

  /*  
    QString newname = Paths::findReadableFile (m_param->GetPlugin(),
                      QStringList() << Paths::Get().GetBinaryPluginPath()
                              << Paths::Get().GetExecutablePath(),
#ifdef LINUX
                      QStringList() << ".so",
                      QStringList() << "lib"
#else
                      QStringList() << ".dll"
#endif
                                              );

    if (newname.isEmpty())
        throw ApexStringException("Cannot find controller plugin: " + m_param->GetPlugin());
    
    qDebug("Plugincontroller: looking for %s", qPrintable (newname));

    // load plugin into pluginfilterinterface
    QPluginLoader loader (newname);
    loader.load();
    if (!loader.isLoaded())
        throw ApexStringException ("Cannot load controller plugin: " +
                                   loader.errorString());


    PluginControllerCreator *creator = qobject_cast<PluginControllerCreator*>
                                       (loader.instance());
    if (!creator)
        throw ApexStringException ("controller plugin has wrong type: " +
                                   loader.errorString());

    m_plugin.reset (creator->createController (m_param->GetPlugin()));
    if (! m_plugin.get())
        throw ApexStringException ("controller could not be created");*/
}

bool apex::device::PluginController::SetParameter(const QString & type, const int channel, const QVariant & value)
{
    Q_ASSERT( m_plugin.get());
    return m_plugin->setParameter(type, channel, value.toString());
}

void apex::device::PluginController::Reset()
{
    Q_ASSERT( m_plugin.get());
    m_plugin->resetParameters();
}

void apex::device::PluginController::Prepare()
{
    Q_ASSERT( m_plugin.get());
    m_plugin->prepare();
}


