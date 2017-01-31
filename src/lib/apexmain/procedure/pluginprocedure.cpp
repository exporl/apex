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
 
#include "pluginprocedure.h"
#include "pluginprocedureapi.h"
#include "pluginprocedurewrapper.h"

#include "services/mainconfigfileparser.h"
#include "services/paths.h"

#include "screen/screenresult.h"

//from libtools
#include "procedure/pluginprocedureparameters.h"

#include <QtScript>

namespace apex
{

PluginProcedure::PluginProcedure ( ExperimentRunDelegate& p_rd, data::ApexProcedureConfig* config ) :
                ApexProcedure ( p_rd, config ),
                m_plugin(new PluginProcedureWrapper),
                m_api(new PluginProcedureAPI(this))
{
    //Qt < 4.6 does not seed javascript Math.random(), do it manually
    //TODO remove if it works in 4.6
#if QT_VERSION < 0x040600
    qsrand(time(0));
#endif

    // construct script filename
    QString scriptname = ((data::PluginProcedureParameters*)m_procedureConfig->GetParameters())->GetScript();
    m_doDebug = ((data::PluginProcedureParameters*)m_procedureConfig->GetParameters())->GetDebugger();

    QString newname = Paths::findReadableFile (((data::PluginProcedureParameters*)
                m_procedureConfig->GetParameters())->GetScript(),
        QStringList() << Paths::Get().GetNonBinaryPluginPath(),
        QStringList() << ".js");

    if (m_doDebug) {
        m_scriptEngineDebugger.attachTo(&m_scriptEngine);
    }

    if (newname.isEmpty())
        throw ApexStringException("Cannot find procedure plugin file " +
                ((data::PluginProcedureParameters*)
                m_procedureConfig->GetParameters())->GetScript() );

    qDebug("Looking for %s", qPrintable (newname));

    qDebug("Adding PluginProcedureInterface object to QtScript");
    QScriptValue baseClass = m_scriptEngine.scriptValueFromQMetaObject<PluginProcedureInterface>();
    m_scriptEngine.globalObject().setProperty("pluginProcedureInterface", baseClass );

    QScriptValue apexTrial = m_scriptEngine.scriptValueFromQMetaObject<data::ApexTrial>();
    m_scriptEngine.globalObject().setProperty("ApexTrial", apexTrial);

    // create API
    QScriptValue APIObject = m_scriptEngine.newQObject(m_api.get());
    m_scriptEngine.globalObject().setProperty("api", APIObject);

    // load script, script is supposed to define pluginProcedure, which inherits from pluginProcedureInterface
    QFile file(newname);
    if (! file.open(QIODevice::ReadOnly) ) {
        throw ApexStringException("Cannot open plugin procedure script: " + newname);
    }
    QString scriptdata(file.readAll());


    // attach the main script library to the current script
    int nApiLines=0;          // number of lines in the API, used to calculate correct error message line
    QString mainPluginLibrary(
        MainConfigFileParser::Get().GetPluginScriptLibrary());
    if ( ! mainPluginLibrary.isEmpty() ) {
        QFile file(mainPluginLibrary);
        if (! file.open(QIODevice::ReadOnly) )
        {
            throw ApexStringException("Cannot open main script library file: " +
                    mainPluginLibrary);
        }

        //QString append = file.readAll();
        m_scriptEngine.evaluate(file.readAll());

    }
     /*   nApiLines=append.count("\n")+1;
        
        scriptdata=append + "\n" + scriptdata;
        file.close();
    }*/

    // set procedureparameters as properties of params object
    qDebug("Setting properties");
    QScriptValue paramsObject = m_scriptEngine.newObject();
    m_scriptEngine.globalObject().setProperty("params", paramsObject);
    data::PluginProcedureParameters* pluginProcedureParameters = ((data::PluginProcedureParameters*)m_procedureConfig->GetParameters());
    const data::tPluginProcedureParameterList& pp = pluginProcedureParameters->GetCustomParameters();
    for (data::tPluginProcedureParameterList::const_iterator it=pp.begin(); it!=pp.end(); ++it) {
        paramsObject.setProperty(it->name, QScriptValue(&m_scriptEngine, it->value));
    }
    paramsObject.setProperty("choices", QScriptValue(&m_scriptEngine, pluginProcedureParameters->GetChoices().nChoices));
    paramsObject.setProperty("presentations", QScriptValue(&m_scriptEngine, pluginProcedureParameters->GetPresentations()));
    QString orderString = (pluginProcedureParameters->GetOrder()==data::ApexProcedureParameters::ORDER_RANDOM)            ?(QString("random")):(QString("sequential"));
    paramsObject.setProperty("order", QScriptValue(&m_scriptEngine, orderString));

    qDebug("Evaluating script");
    m_classname = m_scriptEngine.evaluate(scriptdata);
    file.close();


    // check for errors from script execution
    if (m_scriptEngine.hasUncaughtException()) {
        int lineNo = m_scriptEngine.uncaughtExceptionLineNumber();
        if (lineNo<nApiLines)
            throw ApexStringException(
                    QString("Error in pluginProcedure API line %1: %2")
                    .arg(lineNo).arg(m_classname.toString()));
        else {
            //qDebug() << scriptdata;
            throw ApexStringException(
                    QString("Error in pluginProcedure line %1: %2")
                    .arg(lineNo-nApiLines).arg(m_classname.toString()));
        }
    }

    // give the wrapper some information
    m_plugin->setEngine(&m_scriptEngine);
    m_plugin->setClassname(&m_classname);
    if (! m_plugin->isValid()) {
        throw ApexStringException("Plugin is not valid");
    }

    // Change instance name to something we know:
    m_scriptEngine.globalObject().setProperty(PluginProcedureWrapper::INSTANCENAME, m_classname );

    QString parameterCheckResult = m_plugin->CheckParameters();
    if (!parameterCheckResult.isEmpty())
        throw ApexStringException(parameterCheckResult);
}

PluginProcedure::~PluginProcedure()
{
}

QString PluginProcedure::GetResultXML() const
{
    Q_ASSERT(m_plugin.get());
    return m_plugin->GetResultXML();
}

unsigned PluginProcedure::GetNumTrials() const
{
    Q_ASSERT(m_plugin.get());
    return m_plugin->GetNumTrials();
}

unsigned PluginProcedure::GetProgress() const
{
    qDebug("Fixme: getprogress not implemented in pluginprocedure");
    return 0;
}

void PluginProcedure::StartOutput()
{
    qDebug() << "StartOutput called by" << sender();
    if (m_outputList.empty()) {
            throw ApexStringException("Output list empty: check whether your plugin added a stimulus to the output list");
    }
    ApexProcedure::StartOutput();
}

bool PluginProcedure::NextTrial ( const bool p_answer, const ScreenResult* screenresult  )
{
    // convert ScreenResult to QVariantMap
    QVariantMap s;
    for (ScreenResult::const_iterator it=screenresult->begin(); it!=screenresult->end(); ++it) {
        s.insert( it.key(), it.value());
    }

    QString trial = m_plugin->NextTrial(p_answer, s);

    if (trial.isEmpty() ) {
        ExperimentFinished();
        return false;
    }

    // check whether trial exists
//     if (!isTrial(trial)) {
//         throw ApexStringException("PluginProcedure: requested trial " + trial + " not found");
//     }

    // tell apex that there's a new trial
    NewTrial(trial);
    return true;
}

void PluginProcedure::FirstTrial()
{
    QString trial = m_plugin->FirstTrial();

    if (trial.isEmpty()) {
        ExperimentFinished();
        return;
    }

    // check whether trial exists
    if (!isTrial(trial)) {
        throw ApexStringException("PluginProcedure: requested first trial " + trial + " not found");
    }

    // tell apex that there's a new trial
    NewTrial(trial);
    return;
}

bool PluginProcedure::isTrial(const QString& name)
{
    const data::tTrialList& temp = m_procedureConfig->GetTrials();

    for (data::tTrialList::const_iterator i = temp.begin(); i != temp.end(); ++i)
    {
        if ((*i)->GetID()==name)
            return true;
    }

    return false;
}

} // namespace apex
