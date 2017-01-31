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

//#define PRINTCORRECTOR

#include "corrector.h"
#include "comparator.h"
#include "screen/screen.h"
#include "procedure/apextrial.h"
#include "screen/apexscreenresult.h"
#include "apexcontrol.h"

using namespace apex;

Corrector::Corrector (ExperimentRunDelegate& p_rd, const data::CorrectorData* data) :
    ApexModule(p_rd),
    data (data),
    m_currentResult (false)
{
}

bool Corrector::Correct (const ApexScreenResult& p_answer)
{
    #ifdef SHOWSLOTS
    qDebug ("Slot ApexCorrector::Answer( const QString & p_answer )");
    #endif


    // determine answer_element
    QString answerElement=ApexControl::Get().GetCurrentAnswerElement();

    // get wanted element from screen
    QString wElement;

    ApexScreenResult::const_iterator p = p_answer.find (answerElement);
    bool result = false;
    if (p != p_answer.end()) {
        wElement = p.value();
        m_currentAnswer = wElement;
        m_correctAnswer = GetCorrectAnswer (0);
        result = Compare (GetCorrectAnswer (0), wElement);
    } else {
        //Q_ASSERT(0);
        //throw(1);       // FIXME
        //return; [ stijn ] removed to avoid crashes when no button is pressed
    }

#ifdef PRINTCORRECTOR
    qDebug (answerElement + ": correct(" + m_correctAnswer+ ") =? answer(" + wElement +")");
    if (result) {
        qDebug("Correct");
    } else {
        qDebug("Wrong");
    }
#endif

    m_currentResult = result;
    return result;
}

void Corrector::SetComparator (Comparator* comparator)
{
    this->comparator.reset (comparator);
}

bool Corrector::Compare (const QString& m1, const QString& m2)
{
    return comparator->Compare (m1, m2);
}

void Corrector::SetCorrectAnswer (const unsigned)
{
    //qFatal ("ApexCorrector::SetCorrectAnswer should not be called");
    // NOP
}

QString Corrector::GetResultXML() const
{
    QString temp ("<corrector>\n<result>");
    if (m_currentResult)
        temp += "true";
    else
        temp += "false";
    temp += "</result>\n";

    temp += comparator->GetResultXML();

    temp += "<answer>" + m_currentAnswer + "</answer>\n";
    temp += "<correctanswer>" + m_correctAnswer + "</correctanswer>\n";
    temp += "</corrector>";
    return temp;
}

/**
 * @param position only makes sense for fcacorrector
 * @return  the correct answer for the current trial
 */
const QString Corrector::GetCorrectAnswer (unsigned) const
{
    return (QString) ApexControl::Get().pGetCurrentTrial()->GetAnswer();
}
