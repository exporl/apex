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

#include "apexdata/experimentdata.h"

#include "apextools/apexpaths.h"

#include "common/pluginloader.h"

#include "commongui/listpicker.h"

#include "experimentparser.h"
#include "pluginrunner.h"

#include <QButtonGroup>
#include <QGridLayout>
#include <QPluginLoader>

using namespace cmn;

namespace apex
{

PluginRunner::~PluginRunner()
{
}

void PluginRunner::selectFromDir(const QString & /*path*/)
{
}

bool PluginRunner::select(const QString &pname)
{
    Q_FOREACH (PluginRunnerCreator *pluginCreator,
               PluginLoader().availablePlugins<PluginRunnerCreator>())
        Q_FOREACH (const QString &name, pluginCreator->availablePlugins())
            creators.insert(name, pluginCreator);

    QString pluginName = pname.isEmpty() ? selectPluginRunner() : pname;
    if (pluginName.isEmpty()) {
        qCDebug(APEX_RS, "No plugin selected");
        return false;
    }

    PluginRunnerCreator *creator = creators.value(pluginName);
    if (!creator) {
        Q_EMIT errorMessage(
            "PluginRunner",
            tr("Cannot find a plugin named %1").arg(pluginName));
        return false;
    }

    interface.reset(creator->createRunner(pluginName));
    if (!interface) {
        Q_EMIT errorMessage(
            "PluginRunner",
            tr("Cannot creater plugin interface %1").arg(pluginName));
        return false;
    }

    const QString file = interface->getFileName();
    if (file.isEmpty())
        return true;

    try {
        ExperimentParser parser(file);
        Q_EMIT selected(parser.parse(true, true));
    } catch (std::exception &e) {
        qCDebug(APEX_RS,
                "Pluginrunner: unable to parse %s as an experiment: %s",
                qPrintable(file), e.what());
        Q_EMIT selected(file);
    }
    return true;
}

QString PluginRunner::selectPluginRunner()
{
    ListPicker listPicker(QSL("Select pluginrunner: "));
    Q_FOREACH (const QString &interface, creators.keys())
        listPicker.addItem(interface);
    listPicker.exec();
    return listPicker.selectedItem();
}
}
