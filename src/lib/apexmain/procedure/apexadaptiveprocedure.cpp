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

#include "exceptions.h"

//#define DEBUGADP voor debugging
//#define DEBUGADP

#include "apexadaptiveprocedure.h"
#include "apexcontrol.h"

#include "stimulus/stimulus.h"
#include "stimulus/stimulusparameters.h"
#include "runner/experimentrundelegate.h"
#include "parameters/parametermanager.h"

#include "apextools.h"

//from libdata
#include "experimentdata.h"
#include "procedure/apexadaptiveprocedureparameters.h"

#include <vector>
#include <cmath>                        // abs(double)

namespace apex
{

using namespace stimulus;
using data::StimulusParameters;

/**
 * Check whether we are adapting a variable parameter
 * @return
 */
bool ApexAdaptiveProcedure::CheckAdaptVariableParameter( ) const
{
    if (! m_CheckAdaptVariableParameter_isCached )
    {
        m_CheckAdaptVariableParameter_cache = ! ApexControl::Get().GetCurrentExperiment().isFixedParameter(m_param->m_adapt_parameters.first());
        m_CheckAdaptVariableParameter_isCached=true;

#ifdef DEBUGADP
        if (m_CheckAdaptVariableParameter_cache)
        {
            qDebug("ApexAdaptiveProcedure: using variable parameter");
        }
        else
        {
            qDebug("ApexAdaptiveProcedure: using fixed parameter");
        }
#endif
    }

    // search param->m_adapt_parameter in fixed parameter map
    return m_CheckAdaptVariableParameter_cache;
}

ApexAdaptiveProcedure::ApexAdaptiveProcedure(ExperimentRunDelegate& p_rd,
                                             data::ApexProcedureConfig* config,
                                             bool deterministic) :
        ApexConstantProcedure(p_rd, config, deterministic),
        m_currentProgress(0),
        m_lastSelectedStimulus(0),
        m_CheckAdaptVariableParameter_cache(true),
        m_CheckAdaptVariableParameter_isCached(false)
{
    m_param = (data::ApexAdaptiveProcedureParameters*) m_procedureConfig->GetParameters(); // cast is valid because creation by factory
    m_adaptParam = m_param->m_startValue;

    m_nStartStatus = START_START;           // we are just begun

    m_nUp = m_param->m_nUp;
    m_nDown = m_param->m_nDown;
    m_nStop = m_param->m_nStop;
    m_bLargerIsEasier = m_param->m_bLargerIsEasier;

    m_nCorrect = 0;         // number of correct answers
    m_nWrong = 0;
    m_nReversals = 0;
    m_bMoveUp = false;
    m_bSaturation = false;          // has saturation occured?

    m_adaptParam = m_param->m_startValue;
#ifdef DEBUGADP
    qDebug("Setting m_adaptParam to start value %g", m_adaptParam);
#endif

    // check if the given adaptparams are of the right type
    QStringList::const_iterator it=m_param->m_adapt_parameters.begin();
    ++it;
    for (;it!=m_param->m_adapt_parameters.end(); ++it)
    {
        if (ApexControl::Get().GetCurrentExperiment().isFixedParameter(*it))
            throw ApexStringException(tr("Error: only the first parameter to be adapted can be a fixed parameter"));
    }


    m_bDoSaturateDown=false;
    m_bDoSaturateUp=false;

    m_stepsizes = m_param->m_stepsizes;                     // FIXME copy constructor ok?
    m_changestepsize_type = m_param->m_changestepsize_type;
    UpdateStepSize();

    m_started=false;
}

void ApexAdaptiveProcedure::FindMinMaxParam(data::ApexTrial* p_trial)
{
#ifdef PRINTPROCEDURE
    qDebug("FindMinMaxParam");
#endif
    if ( CheckAdaptVariableParameter() )
    {
        if ( m_param->m_defMaxValue)
        {
            m_bDoSaturateUp=true;
            m_maxParam = m_param->m_maxValue;
#ifdef PRINTPROCEDURE
            qDebug("Set maximum value to %g", m_maxParam);
#endif
        }
        else
        {
            m_bDoSaturateUp=false;
        }
        if ( m_param->m_defMinValue)
        {
            m_bDoSaturateDown=true;
            m_minParam = m_param->m_minValue;
#ifdef PRINTPROCEDURE
            qDebug("Set minimum value to %g", m_minParam);
#endif
        }
        else
        {
            m_bDoSaturateDown=false;
        }
    }
    else                          // fixed parameter, we find the extreme values
    {
        m_bDoSaturateUp=true;
        m_bDoSaturateDown=true;

        // for each stimulus
        tStimulusList::const_iterator it = p_trial->GetStimulusIterator();

        data::t_adaptParam min = m_rd.GetStimulus(*it)->GetFixParameters()->value( m_param->m_adapt_parameters.first() ).toDouble();
        data::t_adaptParam max = min;

        for (; it!=p_trial->GetStimuli().end(); ++it)
        {
            Stimulus* currentStimulus = m_rd.GetStimulus( (*it) );
            data::t_adaptParam currentValue = currentStimulus->GetFixParameters()->value( m_param->m_adapt_parameters.first() ).toDouble();
            if (currentValue>max)
                max=currentValue;
            if (currentValue<min)
                min=currentValue;
        }

        if ( m_param->m_defMaxValue && m_param->m_maxValue < max)
            m_maxParam = m_param->m_maxValue;
        else
            m_maxParam=max;

        if ( m_param->m_defMinValue && m_param->m_minValue > min)
            m_minParam = m_param->m_minValue;
        else
            m_minParam=min;



#ifdef DEBUGADP
        qDebug("Set m_minParam=%g, m_maxParam=%g", m_minParam, m_maxParam);
#endif
    }
}

ApexAdaptiveProcedure::~ApexAdaptiveProcedure()
{
}

bool ApexAdaptiveProcedure::NextTrial(bool p_answer, const ApexScreenResult* screenresult)
{
#ifdef SHOWSLOTS
    qDebug("Slot ApexAdaptiveProcedure::NextTrial(bool)");
#endif

    if (m_bFinished)
        return true;

    ApexProcedure::NextTrial(p_answer, screenresult);

    // set status bar
    switch (m_param->m_bStopAfterType)
    {
        case data::ApexAdaptiveProcedureParameters::STOP_REVERSALS:
        m_currentProgress=m_nReversals+1;
        emit SetProgress(m_currentProgress-1);
        break;
        case data::ApexAdaptiveProcedureParameters::STOP_TRIALS:
        case data::ApexAdaptiveProcedureParameters::STOP_ABOVE:
        m_currentProgress=m_nTrialCount+1;
        emit SetProgress(m_currentProgress);
        break;
    }



    if (m_started)                  // don't adapt the parameter the first time
    {
        m_prevReversals = m_nReversals;
        m_bSaturation=false;
        m_adaptParam = GetNextTargetParam(p_answer,
                m_nStartStatus != START_FIRST || !m_param->m_bFirstUntilCorrect || p_answer != false);
        if (m_bSaturation)
            emit SendMessage( "Saturation" );

        if (m_param->m_bStopAfterType == data::ApexAdaptiveProcedureParameters::STOP_REVERSALS && (m_nReversals >= m_nStop))  //Is the experiment completed ?
        {
#ifdef PRINTPROCEDURE
            qDebug("ApexAdaptiveProcedure::NextTrial: stopping procedure after n reversals");
#endif
            ExperimentFinished();
            return true;


        }
        else if (m_param->m_bStopAfterType == data::ApexAdaptiveProcedureParameters::STOP_TRIALS && (m_nTrialCount >= m_nStop))
        {
#ifdef PRINTPROCEDURE
            qDebug("ApexAdaptiveProcedure::NextTrial: stopping procedure after n trials");
#endif
            ExperimentFinished();
            return true;
        }

    }
    else
        m_started=true;

#ifdef DEBUGADP
    qDebug("New target parameter: %g", m_adaptParam);
#endif


    data::ApexTrial* nextTrial = GetTrial(p_answer);
    if (!nextTrial) return true;

    FindMinMaxParam(nextTrial);

    Stimulus* nextStimulus;
    if (m_nStartStatus==START_FIRST && m_param->m_bFirstUntilCorrect && p_answer==false && m_lastSelectedStimulus)
    {
        Q_CHECK_PTR(m_lastSelectedStimulus);
        nextStimulus = m_lastSelectedStimulus;
        SetVarParam(m_adaptParam);
    }
    else
    {
        nextStimulus = GetStimulus(m_adaptParam, nextTrial);
        m_lastSelectedStimulus = nextStimulus;
    }
    Q_CHECK_PTR(nextTrial);
    Q_CHECK_PTR(nextStimulus);


    NewTrial(nextTrial->GetID());                                           // replaced emit
    emit NewScreen(nextTrial->GetScreen());


    CreateOutputList(nextTrial, nextStimulus);
    /*if (m_bStarted)
            StartOutput();*/


    /*qDebug("Next trial: %s", nextTrial->GetID().ascii());
    qDebug("Next stimulus: %s", nextStimulus->GetID().ascii());*/

    return true;
}




data::ApexTrial* ApexAdaptiveProcedure::GetTrial(bool p_answer)
{
    if (m_useTrialList)
    {

        if (m_nStartStatus==START_START)
            m_nStartStatus=START_FIRST;
        else if (m_nStartStatus==START_FIRST)
        {
            if ( m_param->m_bFirstUntilCorrect && p_answer==false )
            {
#ifdef DEBUGADP
                qDebug("m_bFirstUntilCorrect==true -> not advancing");
#endif
                --m_nTrialCount;
                return *m_trialListIterator;
            }
            else
            {
                m_nStartStatus = START_NORMAL;
                ++m_trialListIterator;
            }
        } else {
            ++m_trialListIterator;
        }



        if (m_trialListIterator == m_trialList.end())
        {
            if ( m_param->m_bStopAfterType == data::ApexAdaptiveProcedureParameters::STOP_ABOVE)
            {
#ifdef PRINTPROCEDURE
                qDebug("ApexAdaptiveProcedure::GetTrial: trial list empty, stopping experiment");
#endif
                ExperimentFinished();
                return NULL;
            }
            else if (m_param->m_bStopAfterType == data::ApexAdaptiveProcedureParameters::STOP_TRIALS
                     || m_param->m_bStopAfterType == data::ApexAdaptiveProcedureParameters::STOP_REVERSALS)
            {
                CreateTrialList(false);
                m_trialListIterator = m_trialList.begin();
            }
            else
            {
                Q_ASSERT(0 && "Experiment should have been stopped");
            }
        }

        data::ApexTrial* next = *m_trialListIterator;
        return next;
    }
    else
    {
        Q_ASSERT(0);
    }

    return NULL;
}


/**
 * abs function used below if it doesn't exist yet for the t_adaptParam type
 * @param p
 * @return
 */
data::t_adaptParam abs(data::t_adaptParam p)
{
    if (p<0)
        return -p;
    else
        return p;
};

/**
 * Select a stimulus from the given trial with the correct value of the parameter to adapt
 * @param p_target
 * @param p_trial
 * @return
 */
Stimulus* ApexAdaptiveProcedure::GetStimulus(data::t_adaptParam p_target, data::ApexTrial* p_trial)
{
#ifdef PRINTPROCEDURE
    qDebug("ApexAdaptiveProcedure::GetStimulus: processing trial %s", p_trial->GetID().ascii());
#endif

    if (CheckAdaptVariableParameter())
    {
        try
        {
            Stimulus* newStim = m_rd.GetStimulus(p_trial->GetStimulus());

            SetVarParam(p_target);

            return newStim;

        }
        catch (...)
        {
            qDebug("Error: there should be only one stimulus per trial");
            throw ApexException();
        }

    }
    else            // adapt fixed parameter
    {
#ifdef DEBUGADP
        qDebug("ApexAdaptiveProcedure::GetStimulus: Adapting fixed parameter %s",
                qPrintable(m_param->m_adapt_parameters.first()));
#endif


        Stimulus* result=SelectStimulusFromList(p_trial->GetStimuli(),
                p_target);

        SetVarParam(p_target);     // set remaining variable parameters

        return result;
        //}

    }
}


Stimulus* ApexAdaptiveProcedure::SelectStimulusFromList(
        const tStimulusList& list,
        data::t_adaptParam p_target) {
        // search stimulus with the best target parameter

    std::vector<Stimulus*> results;

    QString stimName = *(list.begin());
    Stimulus* currentStimulus = m_rd.GetStimulus( stimName );
    QString value = currentStimulus->GetFixParameters()->value( m_param->m_adapt_parameters.first() ).toString();


    data::t_adaptParam maxValue=abs(value.toDouble()-p_target);
    Stimulus* maxStimulus = currentStimulus;

    for (tStimulusList::const_iterator it = list.begin(); it!=list.end(); ++it)
    {
        currentStimulus = m_rd.GetStimulus( (*it) );
#ifdef DEBUGADP
        qDebug("Checking fixed parameter for stimulus %s",
               qPrintable(currentStimulus->GetID()));
#endif
        QString sValue = currentStimulus->GetFixParameters()->value( m_param->m_adapt_parameters.first() ).toString();
        data::t_adaptParam value=sValue.toDouble();

        if (value == p_target)
            results.push_back(currentStimulus);
        else if (abs(value-p_target)<maxValue)
        {
            maxValue = abs(value-p_target);
            maxStimulus = currentStimulus;
        }
    }


    if (results.empty())
    {
#ifdef DEBUGADP
        qDebug("Finding all stimuli at the minimal distance from the target %s",
                qPrintable(maxStimulus->GetFixParameters()->value( m_param->m_adapt_parameters.first()).toString()));
#endif

            // find all stimuli that are maxValue apart from what we're looking for
        results.clear();
        for (tStimulusList::const_iterator it=list.begin(); it!=list.end(); ++it)
        {
            currentStimulus = m_rd.GetStimulus( (*it) );
            QString sValue = currentStimulus->GetFixParameters()->value( m_param->m_adapt_parameters.first() ).toString();
            data::t_adaptParam value=sValue.toDouble();

            if (abs(value-p_target)==maxValue)
            {
                results.push_back(currentStimulus);
            }
        }

        Q_ASSERT  (!results.empty());

//                       return maxStimulus;
    } //else {        // return random stimulus from the list
#ifdef DEBUGADP
    qDebug("Returning random stimulus from the list containing %u items", unsigned(results.size()));
#endif
    Stimulus* result = results.at( ApexTools::RandomRange((int)results.size()-1) );
#ifdef DEBUGADP
    qDebug("Parameter value: %s",
            qPrintable(result->GetFixParameters()->value(m_param->m_adapt_parameters.first()).toString()));
#endif

    return result;
}

/**
 * Set the right variable parameter to value
 * @param stim
 * @param value
 */
void ApexAdaptiveProcedure::SetVarParam(data::t_adaptParam value)
{

    QStringList::const_iterator it = m_param->m_adapt_parameters.begin();
    if (!CheckAdaptVariableParameter())     // the first parameter is fixed and should not be set
        ++it;

    for (; it!=m_param->m_adapt_parameters.end();++it)
    {
//        qDebug("Setting parameters %s to %f", qPrintable(*it), value);
        m_rd.GetParameterManager()->setParameter(*it, value, true);
    }

}

void apex::ApexAdaptiveProcedure::ExperimentFinished( )
{
    ApexProcedure::ExperimentFinished();
}

QString apex::ApexAdaptiveProcedure::XMLBody( ) const
{
    QString result = ApexConstantProcedure::XMLBody();

    if (m_nReversals!=0 && (m_nReversals-m_prevReversals)!=0 )
        result+="\t<reversal/>\n";

    result+="\t<parameter>"+ QString::number(m_adaptParam)+"</parameter>\n";
    result+="\t<stepsize>"+ QString::number(m_currentStepSize)+"</stepsize>\n";
    result+="\t<reversals>"+ QString::number(m_nReversals)+"</reversals>\n";
    result+="\t<saturation>"+ (m_bSaturation?QString("true"):QString("false")) +"</saturation>\n";
    result+="\t<presentations>"+ QString::number(m_nTrialCount)+ "</presentations>\n";

    return result;
}

QString apex::ApexAdaptiveProcedure::XMLHeader( )  const
{
    return QString("<procedure type=\"adaptiveProcedure\" %1>\n").arg(XMLID());
}

QString apex::ApexAdaptiveProcedure::GetEndXML( ) const
{
    // create an extra virtual trial with the last value of the staircase
    QString result;

    //result += "\t<parameter>"+m_adaptParam +"</parameter>\n";
    result += "<trial id=\"VIRTUAL_TRIAL\" type=\"virtual\">\n";
    result += "<!-- This is not a real trial, but a virtual trial based on the previous-->\n";
    result += XMLHeader();
    result += XMLBody();
    result += XMLFooter();
    result += "\n</trial>\n";


    return result;
}

unsigned ApexAdaptiveProcedure::GetNumTrials( ) const
{
#ifdef PRINTPROCEDURE
    qDebug("ApexAdaptiveProcedure::GetNumTrials");
#endif
    switch (m_param->m_bStopAfterType)
    {
        case data::ApexAdaptiveProcedureParameters::STOP_REVERSALS:
        case data::ApexAdaptiveProcedureParameters::STOP_TRIALS:
        return m_nStop;
        break;
        case data::ApexAdaptiveProcedureParameters::STOP_ABOVE:
        return ApexConstantProcedure::GetNumTrials();
        break;
    default:
        Q_ASSERT(0);
        return 0;
    }
}

unsigned ApexAdaptiveProcedure::GetProgress() const
{
    return m_currentProgress;
}



const tStimulusList ApexAdaptiveProcedure::GetStandards(data::ApexTrial * trial)
{
    // find all standards that correspond to the current fixed parametertrial

    if (CheckAdaptVariableParameter())
        return trial->GetStandards();
    else {
        const tStimulusList& allStd=trial->GetStandards();
        tStimulusList results;

        for (tStimulusList::const_iterator it=allStd.begin(); it!=allStd.end();
             ++it)
        {
            Stimulus* currentStimulus = m_rd.GetStimulus( (*it) );
#ifdef DEBUGADP
            qDebug("Checking fixed parameter for stimulus %s",
                   qPrintable(currentStimulus->GetID()));
#endif
            //QString sValue = currentStimulus->GetFixParameters()->value( m_param->m_adapt_parameters.first() ).toString();
            //data::t_adaptParam value=sValue.toDouble();
            data::t_adaptParam value=currentStimulus->GetFixParameters()->value( m_param->m_adapt_parameters.first() ).toDouble();

            if (value==m_adaptParam) {
                results.push_back(*it);
            }
        }

        qDebug("Returning %d standards with correct fixed parameter",
               results.size());
        if (results.isEmpty()) {
            qDebug("No standards found with correct fixed parameter, returning all");
            return allStd;
        }
        return results;
    }
}

}

