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

#include "apextrainingprocedure.h"
#include "screen/screenresult.h"
#include "screen/apexscreen.h"
#include "apextools.h"
#include "apexcontrol.h"

#include "runner/experimentrundelegate.h"
#include "gui/mainwindow.h"     // enablestop

#include <qmessagebox.h>

// TODO: check whether there is a stimulus for each button

namespace apex
{

ApexTrainingProcedure::ApexTrainingProcedure(ExperimentRunDelegate& p_rd,
                                             data::ApexProcedureConfig* config,
                                             bool deterministic) :
    ApexProcedure (p_rd, config)
{
    if (deterministic)
        randomGenerator.setSeed(0);
}


ApexTrainingProcedure::~ApexTrainingProcedure()
{
}


bool ApexTrainingProcedure::NextTrial(bool p_answer, const ScreenResult* screenresult) {
#ifdef SHOWSLOTS
        qDebug("SLOT ApexConstantProcedure::NextTrial(bool p_answer)");
#endif
//      qDebug("Slot ApexConstantProcedure::NextTrial(bool)");

        ApexProcedure::NextTrial(p_answer, screenresult);


        if (m_nTrials > m_procedureConfig->GetParameters()->GetPresentations()) {
                ExperimentFinished();
                return true;
        }

        // get last answer
        QString answerElement=ApexControl::Get().GetCurrentAnswerElement();
        const ScreenResult* sr = m_rd.modScreen()->GetLastResult();
        ScreenResult::const_iterator p = sr->find(answerElement);
        QString answer;
        if ( p != sr->end() )  {
                answer = p.value();
        } else {
                throw ApexStringException(tr("TrainingProcedure: could not find "
                        "an answer from the screen. Check whether the default "
                         "answer element is defined."));
        }

        // find trial containing the last answer
        const std::vector<data::ApexTrial*> &trials = m_procedureConfig->GetTrials();

        std::vector<data::ApexTrial*> results;                // put all results in this vector
        std::vector<data::ApexTrial*>::const_iterator i;
        for (i = trials.begin(); i!=trials.end(); ++i) {
                if ( (*i)->GetAnswer().string() == answer) {
                        results.push_back( *i);
//                      break;
                }
        }

        qDebug("Found %i answers corresponding to user input", (int)(results.size()));


        if (!results.size()) {
                QMessageBox::warning(0, "TrainingProcedure", QString(tr("Error: no trial was found corresponding to the user input (%1), please try again")).arg(answer), QMessageBox::Ok, QMessageBox::NoButton);
		emit(EnableScreen(true));   // no stimulus output
                m_rd.GetMainWindow()->EnableStop(true);
                return false;
        } else {
            // select random element from the results vector
            //int select = ApexTools::RandomRange( 0, ((int)results.size())-1);
            int select = randomGenerator.nextUInt(results.size());
            i=results.begin();
            for (int count=0; count<select; ++count)
                ++i;

            Q_ASSERT(i!=results.end());

                m_currentTrial=*i;
                ApexControl::Get().SetCurrentTrial( (*i)->GetID());
                CreateOutputList(m_currentTrial);
                //StartOutput();
                emit NewScreen(m_currentTrial->GetScreen());
        }

        return true;
}

void ApexTrainingProcedure::Start( )
{
    if( !m_bStarted )
    {
        m_bStarted = true;
        emit(EnableScreen(true));   // no stimulus output
        ApexControl::Get().StartTimer();
        return;
    }
    ErrorHandler::Get().addError( "ApexTrainingProcedure::Start()", "Already Started" );
}

void ApexTrainingProcedure::FirstTrial( )
{
    // get and show first screen
    const std::vector<data::ApexTrial*> &temp = m_procedureConfig->GetTrials();
    if (temp.size()==0)
        throw ApexStringException(tr("No trials defined"));
    m_currentTrial=*(temp.begin());
    ApexControl::Get().SetCurrentTrial( (*temp.begin())->GetID());

    emit NewScreen(m_currentTrial->GetScreen());
}

unsigned ApexTrainingProcedure::GetNumTrials() const
{
    return m_procedureConfig->GetParameters()->GetPresentations();
}


unsigned ApexTrainingProcedure::GetProgress() const
{
    return m_nTrials-1;
}

void apex::ApexTrainingProcedure::setStarted(bool p)
{
    m_bStarted=p;
    if (p)
        emit EnableScreen(true);
}

}

