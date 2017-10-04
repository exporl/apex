#include "apexdata/experimentdata.h"

#include "apexdata/procedure/scriptproceduredata.h"

#include "apexdata/screen/screen.h"
#include "apexdata/screen/screensdata.h"

#include "apexdata/stimulus/stimulidata.h"

#include "apextools/version.h"

#include "scriptprocedure.h"
#include "scriptprocedureapi.h"

#include <QDebug>
#include <QFileInfo>
#include <QMessageBox>
#include <QObjectList>

#include <vector>

namespace apex
{

ScriptProcedureApi::ScriptProcedureApi(ProcedureApi *p_api, ScriptProcedure *p)
    : scriptProcedure(p), api(p_api)
{
    Q_ASSERT(p);
    Q_ASSERT(p_api);

    QObject *p_apiObject = dynamic_cast<QObject *>(p_api);
    connect(this, SIGNAL(showMessageBox(QString, QString)), p_apiObject,
            SIGNAL(showMessageBox(QString, QString)));
}

QObjectList ScriptProcedureApi::trials() const
{
    const data::tTrialList &temp =
        scriptProcedure->procedureData()->GetTrials();

    QObjectList o;

    for (data::tTrialList::const_iterator i = temp.begin(); i != temp.end();
         ++i) {
        o.append(*i);
    }

    return o;
}

QStringList ScriptProcedureApi::stimuli() const
{
    return api->stimuli();
}

QVariant ScriptProcedureApi::fixedParameterValue(const QString stimulusID,
                                                 const QString parameterID)
{
    const data::StimulusData *sd = api->stimulus(stimulusID);
    return sd->GetFixedParameters().value(parameterID);
}

QString ScriptProcedureApi::answerElement(const QString trialid)
{
    return api->answerElement(trialid, scriptProcedure->procedureData());
}

QVariantMap ScriptProcedureApi::parameterMap(const QString stimulusID) const
{
    return api->stimulus(stimulusID)->GetVariableParameters().map();
}

QStringList ScriptProcedureApi::parameterNames(const QString stimulusID) const
{
    return api->stimulus(stimulusID)->GetVariableParameters().names();
}

QVariant ScriptProcedureApi::parameterValue(const QString &parameter_id)
{
    return api->parameterValue(parameter_id);
}

void ScriptProcedureApi::registerParameter(const QString &name)
{
    api->registerParameter(name);
}

void ScriptProcedureApi::messageBox(const QString message) const
{
    Q_EMIT showMessageBox(QString("ScriptProcedure message"), message);
}

void ScriptProcedureApi::showMessage(const QString &message) const
{
    api->showMessage(message);
}

void ScriptProcedureApi::abort(const QString &message) const
{
    throw ApexStringException(tr("ScriptProcedure aborted with message: ") +
                              message);
}

QString ScriptProcedureApi::absoluteFilePath(const QString &path)
{
    return QFileInfo(path).absoluteFilePath();
}
}
