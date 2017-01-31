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

#ifndef __PLUGINRUNNER_H__
#define __PLUGINRUNNER_H__

#include "experimentrunner.h"

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
    void Select(const QString& name);
    void SelectFromDir(const QString& path);
    void Run();
    void Finished();

private:
    QString path;
    QMap<QString, PluginRunnerInterface*> interfaces;

public slots:
    void pluginSelected(QAbstractButton*);

signals:
    void errorMessage(const QString&, const QString&);
};

}

#endif
