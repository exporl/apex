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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_RUNNER_PLUGINRUNNER_H_
#define _EXPORL_SRC_LIB_APEXMAIN_RUNNER_PLUGINRUNNER_H_

#include "experimentrunner.h"
#include "pluginrunnerinterface.h"

#include <QMap>

class PluginRunnerInterface;
class PluginRunnerCreator;
class QAbstractButton;

namespace apex
{

class ApexControl;

class PluginRunner : public ExperimentRunner
{
    Q_OBJECT
public:
    ~PluginRunner();

    /**
        * If name is not specified, a dialog will be shown with all available
        * plugins. Otherwise only the plugin matching name will be loaded
        */
    bool select(const QString &name) Q_DECL_OVERRIDE;
    void selectFromDir(const QString &path) Q_DECL_OVERRIDE;

private:
    QString selectPluginRunner();

    QString path;
    QMap<QString, PluginRunnerCreator *> creators;
    QScopedPointer<PluginRunnerInterface> runnerInterface;

signals:
    void errorMessage(const QString &, const QString &);
};
}

#endif
