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
 
#ifndef PLUGINPROCEDURE_H
#define PLUGINPROCEDURE_H

#include "apexprocedure.h"

#include <QScriptEngine>
#include <QScriptEngineDebugger>

class PluginProcedureWrapper;


namespace apex
{

class PluginProcedureAPI;

class ScreenResult;

/** PluginProcedure: interface between Apex and a QtScript plugin procedure
*/
class PluginProcedure : public ApexProcedure
{
public:
    PluginProcedure ( ExperimentRunDelegate& p_rd, data::ApexProcedureConfig* config );

    ~PluginProcedure();

    virtual QString GetResultXML() const;

    virtual unsigned GetNumTrials() const;
    virtual unsigned GetProgress() const;

public slots:
    virtual void StartOutput();
    //virtual void Start();
    //virtual void Stop();                  // moved to ApexProcedure
    bool NextTrial ( const bool p_answer, const ScreenResult* p_screenresult );
    virtual void FirstTrial();

private:
    bool isTrial(const QString& name);

private:
    std::auto_ptr<PluginProcedureWrapper> m_plugin;
    std::auto_ptr<PluginProcedureAPI> m_api;
    QScriptEngine m_scriptEngine;
    QScriptEngineDebugger m_scriptEngineDebugger;
    bool m_doDebug;
    QScriptValue m_classname;
};

}

#endif
