/******************************************************************************
 * Copyright (C) 2010  Job Noorman <jobnoorman@gmail.com>                     *
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

#ifndef _APEX_SRC_PLUGINS_APEXPROCEDURES_PARAMETERADAPTER_H_
#define _APEX_SRC_PLUGINS_APEXPROCEDURES_PARAMETERADAPTER_H_

#include "apexdata/procedure/adaptiveproceduredata.h"

namespace apex
{

struct ParameterAdapterPrivate;

class ParameterAdapter
{
    public:

        ParameterAdapter(const data::AdaptiveProcedureData* parameters);

        ~ParameterAdapter();

        /**
         * Returns the current value of the parameter.
         */
        data::adapting_parameter currentParameter() const;

        float currentStepsize() const;

        /**
          * Returns true if a reversal occured the previous trial
          */
        bool lastReversal() const;

        int nReversals() const;

        /**
         * Updates the current value of the parameter based on the
         * given answer and the given parameter.
         *
         * @param answer        The answer given by the user on the last trial.
         * @param trialNb       The number of the trial the answer was for.
         * @param usedParameter The value of the parameter used in the last trial.
         * @param keepStepSize  Keep the old stepsize (e.g. because this is the
         *                      first trial, repeatFirstUntilCorrect is true and
         *                      the answer was false)
         */
        void updateParameter(const QVariant& answer,
                             int trialNb,
                             data::adapting_parameter usedParameter,
                             bool keepStepSize);

        /**
         * Returns the number of reversals that have already occurred.
         *
         * NOTE: This method shouldn't be here since it is specific to the
         *       current parameter updating strategy.
         */
        int numberOfReversals() const;

        /**
         * Resets the adapter to its original state. Call this when a new
         * procedure is started.
         */
        void reset();

    private:

        void updateStepsize(int trialNb);

        ParameterAdapterPrivate* const d;
};

} //ns apex

#endif
