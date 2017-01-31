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

#ifndef APEXADAPTIVEPROCEDURE_H
#define APEXADAPTIVEPROCEDURE_H

//from libdata
#include "procedure/apexadaptiveprocedureparameters.h"            // t_adaptparam

namespace apex
{

/**
Implements a general adaptive procedure

@author Tom Francart,,,
*/
class AdaptiveProcedure
{
public:
    AdaptiveProcedure();

    ~AdaptiveProcedure();

    data::ApexAdaptiveProcedureParameters::ChangeStepsize e_changestepsize;

    protected:
    data::t_adaptParam GetNextTargetParam(bool p_lastAnswer, bool updateStepSize);
    void UpdateStepSize();


    // procedure parameters

    int m_nUp;                      // type of procedure
    int m_nDown;
    int m_nStop;            // number of reversals before stop
    bool m_bLargerIsEasier;
    std::map<int,float> m_stepsizes;
    int m_changestepsize_type;

    int m_nCorrect;         // number of correct answers
    int m_nWrong;


    int m_nReversals;
    int m_nTrialCount;

    bool m_bMoveUp;
    bool m_bFirstAdaptation;    // has the first adaptation yet occured?
                                // is used to make sure no reversal is reported
                                // before the parameter has ever changed

    bool m_bDoSaturateUp;   // do we saturate?
    bool m_bDoSaturateDown;
    bool m_bSaturation;             // has saturation occured?

    data::t_adaptParam m_maxParam;        // maximum value of parameter
    data::t_adaptParam m_minParam;        // minimum value of parameter

    data::t_adaptParam m_adaptParam;              // current value of parameter
    data::t_adaptParam m_currentStepSize;
};

}

#endif
