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
 
#ifndef APEXAPEXADAPTIVEPROCEDURE_H
#define APEXAPEXADAPTIVEPROCEDURE_H

// #define DEBUGADP voor debugging

//#include "apexprocedure.h"
#include "apexconstantprocedure.h"
#include "adaptiveprocedure.h"
#include "random.h"
//#include "apexadaptiveprocedureparameters.h"

namespace apex {

namespace data
{
class ApexTrial;
class ApexAdaptiveProcedureParameters;
}

/**
@author Tom Francart,,,
*/
class ApexAdaptiveProcedure : public ApexConstantProcedure, AdaptiveProcedure
{
    Q_OBJECT
public:
    ApexAdaptiveProcedure(ExperimentRunDelegate& p_rd,
                          data::ApexProcedureConfig* config,
                          bool deterministic = false);

    ~ApexAdaptiveProcedure();

    //QString GetXMLResults();
    virtual QString GetEndXML() const;

    virtual unsigned GetNumTrials() const;
    virtual unsigned GetProgress() const;

public slots:
    // void Start();                        // implemented in ApexConstantProcedure
    virtual bool NextTrial(const bool result, const ScreenResult* screenresult);

/*signals:
    void Saturated();*/ //replaced by SendMessage


protected:
    //void GetNextTrial();
    //void ApexAdaptiveProcedure::GetNextTrial(t_adaptParam p_adaptParam);   // redundant
    Stimulus* GetStimulus(data::t_adaptParam p_target, data::ApexTrial* p_trial);
    data::ApexTrial* GetTrial(bool p_answer);

    virtual void ExperimentFinished();
    virtual const tStimulusList GetStandards( data::ApexTrial* trial);

private:
    virtual QString XMLHeader() const;
    virtual QString XMLBody() const;

    virtual void SetVarParam(data::t_adaptParam value);

    data::ApexAdaptiveProcedureParameters* m_param;

    bool CheckAdaptVariableParameter() const;
    Stimulus* SelectStimulusFromList(const tStimulusList& list,
            data::t_adaptParam p_target);
    
    void FindMinMaxParam(data::ApexTrial* p_trial);               // set min en max value of parameter

    bool m_started;
    int m_nStartStatus;                     // contains how far we are in startup
                                                            // is currently used to determine whether to keep presenting the first trial if answer is false (speech in noise)

    unsigned m_currentProgress;             // current position on the progress bar

    enum { START_START, START_FIRST, START_NORMAL };
    int m_prevReversals;            // number of reversals in the previous trial

    Stimulus*   m_lastSelectedStimulus;

    Random m_random;

    mutable bool m_CheckAdaptVariableParameter_cache;
    mutable bool m_CheckAdaptVariableParameter_isCached;
};

}

#endif
