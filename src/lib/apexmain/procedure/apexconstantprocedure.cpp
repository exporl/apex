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
 
/*
Constant stimuli procedure for apex 2
*/

#include "apexconstantprocedure.h"
#include "stimulus/stimulus.h"
#include "apexcontrol.h"

#include <QDebug>

namespace apex
{

ApexConstantProcedure::ApexConstantProcedure(ExperimentRunDelegate& p_rd,
                                             data::ApexProcedureConfig* config,
                                             bool deterministic) :
    ApexProcedure (p_rd, config, deterministic)
{
}


bool ApexConstantProcedure::NextTrial(bool p_answer, const ScreenResult* screenresult)
{
#ifdef SHOWSLOTS
    qDebug("SLOT ApexConstantProcedure::NextTrial(bool p_answer), id=" + m_id);
#endif


    if (m_bFinished)
            return true;

    ApexProcedure::NextTrial(p_answer, screenresult);

    GetNextTrial();

    return true;
}

void ApexConstantProcedure::GetNextTrial()
{
#ifdef SHOWSLOTS
    qDebug("SLOT ApexConstantProcedure::GetNextTrial()");
#endif

    if (m_bFinished) {
        Q_ASSERT("ApexProcedure::NextTrial: m_bFinished==true, should not come here");
        return;
    }

    emit SetProgress(m_nTrials-1); //[ stijn ] moved up a bit so progress can complete

    if (m_trialListIterator == m_trialList.end()) {
        ExperimentFinished();
        return;
    }

    data::ApexTrial* nextTrial = *m_trialListIterator;
    ++m_trialListIterator;

    if (!nextTrial)
        return;

    Q_CHECK_PTR(nextTrial);

    // TODO: parameter aanpassen in nextStimulus

    NewTrial( nextTrial->GetID() );
    emit NewScreen(nextTrial->GetScreen());


    CreateOutputList( nextTrial );

    // if( m_bStarted )
        //StartOutput();

    /*qDebug("Next trial: %s", nextTrial->GetID().ascii());
    qDebug("Next stimulus: %s", nextStimulus->GetID().ascii());*/
}

void ApexConstantProcedure::FirstTrial()
{
  m_bStarted = false;
  CreateTrialList();
  NextTrial(false, 0);
  emit(SetNumTrials( GetNumTrials()));
  emit(SetProgress(0));
}

unsigned ApexConstantProcedure::GetNumTrials() const
{
        return (const unsigned) m_trialList.size();
}

unsigned ApexConstantProcedure::GetProgress() const
{
    return m_nTrials-1;
}

}

