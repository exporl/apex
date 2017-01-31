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
 
#ifndef APEXAPEXMULTIPROCEDURE_H
#define APEXAPEXMULTIPROCEDURE_H

#include "apexprocedure.h"
#include "random.h"

namespace apex
{

/**
@author Tom Francart,,,
*/

struct tMultiProcedureMember
{
    ApexProcedure* proc;
    bool lastAnswer;
    unsigned progress;
    unsigned totalprogress;

    tMultiProcedureMember(ApexProcedure* p_proc, bool p_lastAnswer=false): proc(p_proc), lastAnswer(p_lastAnswer) {};
};

class ApexMultiProcedure : public ApexProcedure
{
    //Q_OBJECT
    typedef std::vector<tMultiProcedureMember> tProcList;
public:
    ApexMultiProcedure(ExperimentRunDelegate& p_rd,data::ApexProcedureConfig* config);

    ~ApexMultiProcedure();

    void AddProcedure( ApexProcedure* p_proc);
    virtual QString GetResultXML() const;
    virtual QString GetEndXML() const;

    virtual unsigned GetNumTrials() const;
    virtual unsigned GetProgress() const;

public slots:
    virtual void Start();
    //virtual void Stop();                  // moved to ApexProcedure
    bool NextTrial(const bool p_answer, const ScreenResult* screenresult);
    virtual void FirstTrial();
    virtual void StimulusDone();                            // react on stimulusdone from stimulusoutput
    virtual void RepeatOutput();

protected:
    //void ApexMultiProcedure::GetNextTrial();

    tProcList m_procedures;
    virtual void StartOutput();

private:
    tProcList::iterator m_curProc;                  // current procedure

    void SelectProcedure();

    Random m_random;

    mutable int m_nNumtrials;
};

}

#endif
