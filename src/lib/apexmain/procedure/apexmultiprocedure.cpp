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
Multi stimuli procedure for apex 2
*/

#include "apexmultiprocedure.h"
#include "stimulus/stimulus.h"
#include "apexcontrol.h"
#include "apextools.h"
#include <algorithm>

namespace apex
{

ApexMultiProcedure::ApexMultiProcedure(ExperimentRunDelegate& p_rd, data::ApexProcedureConfig* config):
    ApexProcedure(p_rd,config),
    m_curProc(),
    m_nNumtrials(-1)
{
}


ApexMultiProcedure::~ApexMultiProcedure()
{
// delete procedures
//for_each( m_procedures.begin(), m_procedures.end(), ZeroizingDeleter() );

    for (tProcList::iterator it=m_procedures.begin(); it!=m_procedures.end(); ++it)
    {
        delete (*it).proc;
        (*it).proc=0;
    }

}


bool ApexMultiProcedure::NextTrial(bool p_answer, const ApexScreenResult* screenresult)
{
#ifdef SHOWSLOTS
    qDebug("SLOT ApexMultiProcedure::NextTrial(bool p_answer)");
#endif
    //      qDebug("Slot ApexMultiProcedure::NextTrial(bool)");

    (*m_curProc).lastAnswer=p_answer;

    while (1) {
        SelectProcedure();
        if (m_bFinished)
            return false;


        if (! (*m_curProc).proc->isStarted() ) {
            (*m_curProc).proc->FirstTrial();
            //(*m_curProc).proc->Start();
            (*m_curProc).proc->setStarted(true);        // set started flag, but do not start output yet, because ApexControl will do this because it doesn't know this is the first trial of this procedure
            SetNumTrials(GetNumTrials());
        } else {
            (*m_curProc).proc->NextTrial((*m_curProc).lastAnswer, screenresult);        // FIXME: screenresult is not defined!
        }
        emit(SetProgress(GetProgress()));

        if (m_bFinished || !(*m_curProc).proc->isFinished())
            break;
    }

    return true;
}

void ApexMultiProcedure::FirstTrial()
{
    m_bStarted = false;

    SelectProcedure();
    (*m_curProc).proc->FirstTrial();

    emit(SetNumTrials( GetNumTrials()));
//        qDebug("Selected procedure " + m_curProc->GetID());

    // FIXME: start all procedures so we can get the right value of getnumtrials
}


void ApexMultiProcedure::AddProcedure( ApexProcedure * p_proc )
{
    Q_CHECK_PTR(p_proc);
    m_procedures.push_back(tMultiProcedureMember(p_proc));
}

void ApexMultiProcedure::SelectProcedure( )
{
    if ( m_procedureConfig->GetParameters()->GetOrder() == data::ApexProcedureParameters::ORDER_SEQUENTIAL)
    {


        if (m_bStarted==false)
        {
            m_curProc = m_procedures.begin();
            return;
        }

        for (unsigned i=0; i<m_procedures.size(); ++i)
        {
            ++m_curProc;

            if (m_curProc==m_procedures.end())
                m_curProc = m_procedures.begin();

            if (!(*m_curProc).proc->isFinished())
                return;
        }

        ExperimentFinished();
        return;


    }
    else if ( m_procedureConfig->GetParameters()->GetOrder() == data::ApexProcedureParameters::ORDER_RANDOM)
    {

    // check whether all procedures are finished
        bool finished=true;
        for (tProcList::const_iterator it=m_procedures.begin(); it!=m_procedures.end(); ++it)
        {
            finished &= (*it).proc->isFinished();
        }
        if (finished)
        {
            ExperimentFinished();
            return;
        }

        while (1)
        {
            int number = ApexTools::RandomRange(0, m_procedures.size()-1 );
            qDebug("Selecting procedure #%i", number);

            if (! m_procedures[number].proc->isFinished())
            {
                m_curProc=m_procedures.begin();
                for (int i=0; i<number; ++i)            // set the iterator to the number'est procedure
                    ++m_curProc;
                return;
            }

        }



    }
    else if ( m_procedureConfig->GetParameters()->GetOrder() == data::ApexProcedureParameters::ORDER_ONEBYONE)
    {


        if (m_bStarted==false)
        {
            m_curProc = m_procedures.begin();
            return;
        }

        if ((*m_curProc).proc->isFinished()) {
            ++m_curProc;
            if (m_curProc==m_procedures.end())
                ExperimentFinished();
            return;
        }


        return;




    }
    else
    {
        Q_ASSERT(0&&"Invalid order");
    }
}

void apex::ApexMultiProcedure::Start( )
{
//    ApexProcedure::Start();
    (*m_curProc).proc->Start();

    if( !m_bStarted ) {
        m_bStarted = true;
        return;
    }
    ErrorHandler::Get().addError( "ApexConstantProcedure::Start()", "Already Started" );
}

void ApexMultiProcedure::StartOutput()
{
      (*m_curProc).proc->StartOutput();
// called from ApexProcedure::Start()
}

QString ApexMultiProcedure::GetResultXML( ) const
{
    qDebug("ApexMultiProcedure::GetResultXML: id=%s", qPrintable (m_id));
    return (*m_curProc).proc->GetResultXML();
}

QString ApexMultiProcedure::GetEndXML( ) const
{
    QString result;

    for (tProcList::const_iterator it=m_procedures.begin(); it!=m_procedures.end(); ++it)
    {
        result += (*it).proc->GetEndXML();
    }

    return result;
}


unsigned ApexMultiProcedure::GetNumTrials( ) const
{
    /*if (m_nNumtrials!=-1)
        return m_nNumtrials;*/

// get total number of trials
    m_nNumtrials=0;
    for (tProcList::const_iterator it=m_procedures.begin(); it!=m_procedures.end(); ++it)
    {
        m_nNumtrials += (*it).proc->GetNumTrials();
    }

    return m_nNumtrials;
}

unsigned ApexMultiProcedure::GetProgress() const {

// get total number of trials
    unsigned progress=0;
    for (tProcList::const_iterator it=m_procedures.begin(); it!=m_procedures.end(); ++it)
    {
        progress += (*it).proc->GetProgress();
    }

    return progress;
}

}

