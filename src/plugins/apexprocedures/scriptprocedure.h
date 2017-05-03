#ifndef _APEX_SRC_PLUGINS_APEXPROCEDURES_SCRIPTPROCEDURE_H_
#define _APEX_SRC_PLUGINS_APEXPROCEDURES_SCRIPTPROCEDURE_H_


#include "apexdata/procedure/procedureinterface.h"
#include "apexdata/procedure/trial.h"

#include "scriptprocedureapi.h"
#include "scriptprocedurewrapper.h"

#include <QScriptEngine>
#include <QScriptEngineDebugger>
#include <QStringList>

class ScriptProcedureWrapper;

namespace apex
{
class ProcedureApi;
class ApexScreenResult; //not in refractory

namespace data
{
class ProcedureData;
class ProcedureParameters;
class ScriptProcedureData;
}

class ScriptProcedure : public ProcedureInterface
{
    Q_DECLARE_TR_FUNCTIONS(ScriptProcedure)
public:
    ScriptProcedure(ProcedureApi* api, const data::ProcedureData *data);
    ~ScriptProcedure();

    double progress() const;
    ResultHighlight processResult(const ScreenResult *screenResult);
    QString resultXml() const;
    QString finalResultXml() const;

    data::Trial setupNextTrial();
    QString firstScreen();

    const data::ScriptProcedureData* procedureData();

private:

    QScopedPointer<ScriptProcedureWrapper> m_plugin;
    QScopedPointer<ScriptProcedureApi> m_api;
    QScriptEngine m_scriptEngine;
    QScriptEngineDebugger m_scriptEngineDebugger;
    bool m_doDebug;
    QScriptValue m_classname;

    const data::ScriptProcedureData* data;
    QString lastAnswer;
    QString lastCorrectAnswer;
    data::Trial lastTrial;

    data::Trial trialProto;
};

}

#endif // _APEX_SRC_PLUGINS_APEXPROCEDURES_SCRIPTPROCEDURE_H_
