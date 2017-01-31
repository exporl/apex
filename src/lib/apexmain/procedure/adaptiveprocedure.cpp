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

#include "adaptiveprocedure.h"

// #define DEBUGADP voor debugging

namespace apex {

AdaptiveProcedure::AdaptiveProcedure()
{
    // some default values, it won't work with these
    m_adaptParam= 0;
    m_nDown = 0;
    m_nUp = 0;
    m_nStop = 0;
    m_nReversals = 0;
    m_bLargerIsEasier=true;
    m_nTrialCount=0;
    m_bMoveUp=false;
    m_bFirstAdaptation=false;
    m_bDoSaturateUp=true;
    m_bDoSaturateDown=true;
    m_currentStepSize=0;
    m_maxParam=0;
    m_minParam=0;
    m_changestepsize_type=data::ApexAdaptiveProcedureParameters::AFTER_REVERSALS;
}

AdaptiveProcedure::~AdaptiveProcedure()
{
}

data::t_adaptParam AdaptiveProcedure::GetNextTargetParam(bool p_lastAnswer, bool updateStepSize)
{
    ++m_nTrialCount;


    //Check wether the response was correct or wrong
    if (p_lastAnswer) { //Correct response
        //m_uaCorrect[m_nNextOutput]++;
        m_nCorrect++;
        m_nWrong=0;                             // orig adaptive
    } else      {       //Wrong response
        m_nWrong++;
        m_nCorrect=0;
    }

    //After a couple of correct or wrong responses the test is altered (adaptive procedure)
    ///////////////////////////////////////////

    if ( m_nCorrect==m_nDown) {
        //We can go one step in the 'Harder' direction (moving down the list of stimuli)
        //////////////////////////////////////////////////////////
        if ( (m_bFirstAdaptation) && (!m_bMoveUp) ) {// i.e. moving down
            //A reversal occured
            m_nReversals++;
        }
        if (updateStepSize)
            UpdateStepSize();
        m_bMoveUp = TRUE;
                if (m_bLargerIsEasier)
                        m_adaptParam = m_adaptParam - m_currentStepSize;
                else
                        m_adaptParam = m_adaptParam + m_currentStepSize;

        m_nCorrect = 0;
        m_bFirstAdaptation=true;

    } else if (m_nWrong==m_nUp) {
        //We can go one step in the 'easier' direction (moving down the list of stimuli)
        if ( (m_bFirstAdaptation) && (m_bMoveUp) ) {
            //A reversal occured
            m_nReversals++;

        }
        if (updateStepSize)
            UpdateStepSize();
        m_bMoveUp = FALSE;
        if (m_bLargerIsEasier)
            m_adaptParam = m_adaptParam + m_currentStepSize;
        else
            m_adaptParam = m_adaptParam - m_currentStepSize;

        m_nWrong=0;
        m_bFirstAdaptation=true;
    } else {
        if (updateStepSize)
            UpdateStepSize();
    }

    if ( (m_adaptParam > m_maxParam) && m_bDoSaturateUp) {
#ifdef DEBUGADP
        qDebug("** Saturation UP: %g>%g", m_adaptParam, m_maxParam);
#endif
        m_bSaturation = TRUE;
        m_adaptParam = m_maxParam;
    } else if ( (m_adaptParam < m_minParam) && m_bDoSaturateDown) {
#ifdef DEBUGADP
        qDebug("** Saturation DOWN: %g<%g", m_adaptParam, m_minParam);
#endif
        m_bSaturation = TRUE;
        m_adaptParam = m_minParam;
    }

    return m_adaptParam;
}

void AdaptiveProcedure::UpdateStepSize()
{
    int current;            // where are we?
    if (m_changestepsize_type==data::ApexAdaptiveProcedureParameters::AFTER_REVERSALS)
        current = m_nReversals;
    else if (m_changestepsize_type==data::ApexAdaptiveProcedureParameters::AFTER_TRIALS)
        current = m_nTrialCount;
    else
        Q_ASSERT(0);

    // look for stepsize after trial m_nTrialCount

    // look for largest begin number <= m_nTrialCount
    for (std::map<int,float>::const_iterator it = m_stepsizes.begin(); it!=m_stepsizes.end(); ++it)
        if ( (*it).first<=current)
            m_currentStepSize = (*it).second;

#ifdef PRINTPROCEDURE
    qDebug("New stepsize: " + QString::number(m_currentStepSize));
    qDebug("m_nTrialCount: " + QString::number(m_nTrialCount));
#endif

    return;
}

}

