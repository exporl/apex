/******************************************************************************
 * Copyright (C) 2010  Job Noorman <jobnoorman@gmail.com>                     *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#ifndef _APEX_SRC_PLUGINS_APEXPROCEDURES_TRAININGPROCEDURE_H_
#define _APEX_SRC_PLUGINS_APEXPROCEDURES_TRAININGPROCEDURE_H_

#include "apexdata/procedure/procedureinterface.h"

#include "apextools/random.h"

namespace apex
{

/*!
 * \warning A TrainingProcedure works in two steps. First the buttons are shown
 * and second the stimulus is presented. This leads to the fact that the
 * argument of processResult(const ScreenResult* screenResult) is wrong,
 * its content will not match with those of the last data::Trial (the stimulus
 * of data::Trial lags one trial).
 * This is solved by resetting the answer of the ScreenResult with the answer of
 * the previous trial. Because the ScreenResult is const, a const_cast will be
 * performed.
 * So after executing processResult(const ScreenResult* screenResult), the
 * ScreenResult will contain the information of the previous trial.
 */

class TrainingProcedure : public ProcedureInterface
{
public:
    TrainingProcedure(ProcedureApi *a, const data::ProcedureData *d,
                      bool deterministic = false);

    data::Trial setupNextTrial();
    QString firstScreen();
    ResultHighlight processResult(const ScreenResult *screenResult);
    double progress() const;
    QString resultXml() const;
    QString finalResultXml() const;

private:
    int trialsDone;
    int totalTrials;
    QString currentTrial;
    QString lastAnswer;
    data::Trial lastTrial;

    QString previousAnswer;

    Random randomGenerator;
};
}

#endif
