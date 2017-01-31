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
 
#include "pluginprocedureapi.h"

#include "pluginprocedure.h"
#include "version.h"
#include "apexcontrol.h"
#include "experimentdata.h"
#include "stimulus/stimulidata.h"
#include "runner/experimentrundelegate.h"
#include "parameters/parametermanager.h"

#include "stimulus/stimulus.h"
#include "stimulus/stimulusparameters.h"

//from libdata
#include "procedure/apextrial.h"

#include <QObjectList>
#include <vector>
#include <QMessageBox>
#include <QDebug>
#include <screen/screensdata.h>
#include <screen/screen.h>



namespace apex {


PluginProcedureAPI::PluginProcedureAPI(PluginProcedure* p):
            pluginProcedure(p)
{
    Q_ASSERT(p);
}


QObjectList PluginProcedureAPI::GetTrials() const
{

    Q_ASSERT(pluginProcedure);
    Q_ASSERT(pluginProcedure->m_procedureConfig);

    const data::tTrialList& temp = pluginProcedure->m_procedureConfig->GetTrials();
    QObjectList o;

    for (data::tTrialList::const_iterator i=temp.begin(); i!=temp.end(); ++i) {
        o.append(*i);
    }

    return o;

}

QStringList PluginProcedureAPI::GetStimuli() const
{
    //qDebug() << pluginProcedure->m_rd.GetData().GetStimuliData().keys();
    return pluginProcedure->m_rd.GetData().stimuliData()->keys();
}

QVariant PluginProcedureAPI::GetFixedParameterValue(const QString stimulusID,
                                const QString parameterID)
{
    return pluginProcedure->m_rd.GetData().
            stimuliData()->operator[](stimulusID).
            GetFixedParameters().value(parameterID);
}


void PluginProcedureAPI::AddToOutputList(const QString& stimulus)
{
    pluginProcedure->m_outputList.push_back(stimulus);
}


void PluginProcedureAPI::CreateOutputList( QString p_trial, QString p_stimulus )
{
    // lookup stimulus by name
    Stimulus* stim;
    try {
        stim = pluginProcedure->m_rd.GetStimulus(p_stimulus);
    } catch (...) {
        throw ApexStringException("PluginProcedure: stimulus " + p_stimulus + " does not exist");
    }

    // lookup trial by name
    Q_ASSERT(pluginProcedure);
    Q_ASSERT(pluginProcedure->m_procedureConfig);
    data::ApexTrial* trial=0;

    const data::tTrialList& temp = pluginProcedure->m_procedureConfig->GetTrials();
    for (data::tTrialList::const_iterator i=temp.begin(); i!=temp.end(); ++i) {
        if ((*i)->GetID()==p_trial) {
            trial=*i;
            break;
        }
    }
    if (!trial) {
        trial = new data::ApexTrial();
        trial->SetID(p_trial);
        //throw ApexStringException("PluginProcedure: trial not found with id: " + p_trial);
    } else {
        ApexControl::Get().SetCurrentTrial(p_trial);     // FIXME: avoid glitch where corrector tries to find current trial
    }

    pluginProcedure->CreateOutputList( trial, stim ) ;
}

void PluginProcedureAPI::createTrial(const QString& trial,
                                     const QString& stimulus,
                                     const QString& screen)
{
    SetScreen(screen);
    CreateOutputList(trial, stimulus);
}

void PluginProcedureAPI::SetParameter(/*const QString& stimulus_id,*/ const QString& parameter_id, const QVariant& value)
{
    /*Stimulus* stim = ApexControl::Get().GetStimulus(stimulus_id);
    StimulusParameters* param = stim->GetVarParameters();
    (*param)[ parameter_id ] = value ;*/
    pluginProcedure->m_rd.GetParameterManager()
            ->setParameter(parameter_id, value, true);
}

QVariant PluginProcedureAPI::GetParameter (const QString& parameter_id)
{
    ParameterManager *manager = pluginProcedure->m_rd.GetParameterManager();
    return manager->parameterValue(parameter_id);
}

void PluginProcedureAPI::ResetParameters() {
    pluginProcedure->m_rd.GetParameterManager()
            ->reset();
}

void PluginProcedureAPI::registerParameter(const QString& name)
{
    data::Parameter paramName("user", name, QVariant(),
                                  data::NO_CHANNEL, true);
    paramName.setId(name);

    pluginProcedure->m_rd.GetParameterManager()->registerParameter(name, paramName);
}

float PluginProcedureAPI::GetApexVersion() const {
    return APEX_VERSION_FLOAT;
}

void PluginProcedureAPI::SetScreen(const QString screen)
{
    pluginProcedure->NewScreen(screen);
}


void PluginProcedureAPI::Finished()
{
    pluginProcedure->Finished();
}

void PluginProcedureAPI::SetProgress(const int value)
{
    pluginProcedure->SetProgress((unsigned)value);
}

void PluginProcedureAPI::SetNumTrials(const int value)
{
    pluginProcedure->SetNumTrials((unsigned)value);
}


QString PluginProcedureAPI::screenElementText(const QString& screenId,
                                              const QString& elementId) const
{
    return ApexControl::Get().GetCurrentExperiment().screensData()
                                ->GetScreen(screenId).elementTextById(elementId);
}


void PluginProcedureAPI::MessageBox(const QString message) const
{
    QMessageBox::information(0, "PluginProcedure message", message);
}

void PluginProcedureAPI::AddWarning(const QString message) const
{
    ErrorHandler::Get().addWarning("PluginProcedure", message);
}

void PluginProcedureAPI::Abort(const QString message) const
{
    throw ApexStringException(tr("PluginProcedure aborted with message: ")
            + message);
}

}
