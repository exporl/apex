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
 
#include "fcacorrector.h"
#include "screen/screenresult.h"
#include "apexcontrol.h"

//from libdata
#include "corrector/correctordata.h"

//from libtools
#include "exceptions.h"

#include <QMessageBox>

using namespace apex;

FCACorrector::FCACorrector (ExperimentRunDelegate& p_rd,const data::CorrectorData* data) :
    Corrector (p_rd, data),
    m_fcaCorrectAnswer (0)
{
}

/**
 * Answer ranges between 1 and <choices>
 * @param p_answer 
 */
void FCACorrector::SetCorrectAnswer (const unsigned p_answer)
{
#ifdef SHOWSLOTS
    qDebug("SLOT ApexFCACorrector::SetCorrectAnswer");
#endif
    
    Q_ASSERT(p_answer > 0);
    m_fcaCorrectAnswer = p_answer;
    m_correctAnswer = QString::number(p_answer);
}

bool FCACorrector::Correct( const ScreenResult & p_answer )
{
#ifdef SHOWSLOTS
    qDebug("SLOT ApexFCACorrector::Answer");
#endif
    
    if (m_fcaCorrectAnswer<=0)
        throw ApexStringException("FCACorrector: could not find the correct answer. Did you set <choices> to the right value? (>1)");

    QString answerElement = ApexControl::Get().GetCurrentAnswerElement();   // AnswerElement();
            
    QString wElement;  
    ScreenResult::const_iterator p = p_answer.map().find(answerElement);
    if ( p != p_answer.map().end() )
            wElement = p.value();
    else {
            throw(ApexStringException(answerElement + tr("could not be found in the result from the screen")));              
    }

    const data::CorrectorData::AnswerMap& answers = data->answers();
    data::CorrectorData::AnswerMap::const_iterator i =
            answers.find (m_fcaCorrectAnswer);
    if (i==answers.end()) {
        QMessageBox::critical(0,tr("Corrector error"), tr("Error: cannot found the correct answer in the list of fca corrector answers"));
        return false;
    }
    //Q_ASSERT (i != answers.end());


	bool result;

	// check whether the answer occurs in the corrector answermap (if it doesn't, the answer cannot be correct)
	data::CorrectorData::AnswerMap::const_iterator it;
	for ( it=answers.begin(); it!=answers.end(); ++it)
		if (it.value()==wElement) break;
	if (it==answers.end()) {
		result=false;
		m_currentAnswer="-1";
	} else {
		result = wElement == i.value();
		m_currentAnswer=QString::number(it.key());
	}


        
#ifdef PRINTCORRECTOR
    if (result) {
        qDebug(" - Correct: " +  answerElement + " == " + wElement );  //FIXME is this ok?
    } else {
        qDebug(" - Wrong: " +  answerElement + " == " + wElement );   //...
    }
#endif
        
    m_currentResult = result;
    return result;
}

/**
 * @param position position for which to return the element. If position is not
 * specified, the correct answer for the current trial is assumed @return  the
 * name of the screen element that is the correct answer for the current trial
 * and the given position.
 */
const QString FCACorrector::GetCorrectAnswer (unsigned position) const
{
    //Q_ASSERT(position>0);
    if (position == 0)
        position = m_fcaCorrectAnswer;
    
    data::CorrectorData::AnswerMap::const_iterator found = data->answers().find (position);
    if (found== data->answers().end())
        throw ApexStringException(tr("Error: invalid position for FCA corrector: the procedure put the stimulus in a position for which there is no answer defined in the FCA corrector."));

    return found.value();
}
