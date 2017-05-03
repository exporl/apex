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

#ifndef _APEX_SRC_PLUGINS_APEXPROCEDURES_CONSTANTPROCEDURE_H_
#define _APEX_SRC_PLUGINS_APEXPROCEDURES_CONSTANTPROCEDURE_H_

#include "apexdata/procedure/procedureinterface.h"

#include <QStringList>

namespace apex
{

class ProcedureApi;

namespace data
{
class ProcedureData;
class ConstantProcedureData;
}

/**
*/
class ConstantProcedure : public ProcedureInterface
{
    public:

        ConstantProcedure(ProcedureApi* api, const data::ProcedureData* data);

        QString firstScreen();
        data::Trial setupNextTrial();
        double progress() const;
        ResultHighlight processResult(const ScreenResult *screenResult);
        QString resultXml() const;
        virtual QString finalResultXml() const;

    private:

        const data::ConstantProcedureData* data;

        QStringList trialList;      //! list of trials to be presented
        int currentTrial;
        int stimulusPosition;       // position of the current stimulus, only valid when data->choices().nChoices() > 1

        bool trialSetup;            //! safety check: only process results after a
                                    // trial has been set up
        bool lastResult;            //! was the last processed result correct or false?
        QString lastAnswer;
        QString lastCorrectAnswer;
        data::Trial lastTrial;
};

}

#endif
