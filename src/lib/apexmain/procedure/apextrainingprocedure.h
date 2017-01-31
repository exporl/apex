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
 
#ifndef APEXAPEXTRAININGPROCEDURE_H
#define APEXAPEXTRAININGPROCEDURE_H

#include "apexprocedure.h"

#include <random.h>

namespace apex
{
namespace data
{
class ApexTrial;
}
/**
Training procedure. Presents a stimulus according to the last answer
When this procedure is used, there should be only one screen (it will work though with more)
@author Tom Francart,,,
*/
class ApexTrainingProcedure : public ApexProcedure
{
public:
    ApexTrainingProcedure(ExperimentRunDelegate& p_rd,
                          data::ApexProcedureConfig* config,
                          bool deterministic = false);

    ~ApexTrainingProcedure();

    //virtual QString GetResultXML();

    virtual unsigned GetNumTrials() const;
    virtual unsigned GetProgress() const;
    virtual bool CanPause() const { return false;};           // pause doesn't make sense for training procedure

    virtual void setStarted(bool p);

public slots:
    virtual void Start();
//      virtual void Stop();

    virtual bool NextTrial(const bool result, const ScreenResult* screenresult);
    /*      virtual void StimulusDone();*/

    virtual void FirstTrial(); //[ stijn ] prepare first trial*/

private:
    data::ApexTrial* m_currentTrial;

    //Random randomGenerator;
};

}

#endif
