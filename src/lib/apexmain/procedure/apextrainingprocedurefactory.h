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

#ifndef APEXAPEXTRAININGPROCEDUREFACTORY_H
#define APEXAPEXTRAININGPROCEDUREFACTORY_H

#include "apexprocedurefactory.h"
#include "apextrainingprocedure.h"
#include "apextrainingprocedureparameters.h"

#include "xml/xercesinclude.h"
namespace XERCES_CPP_NAMESPACE
{
class DOMElement;
};

namespace apex
{

/**
@author Tom Francart,,,
*/
class ApexTrainingProcedureFactory : public ApexProcedureFactory
{
    public:
        ApexTrainingProcedureFactory();

        virtual ~ApexTrainingProcedureFactory();

        virtual ApexTrainingProcedure* MakeProcedure(ExperimentRunDelegate& p_rd, ApexProcedureConfig* config);

        virtual ApexProcedureConfig* MakeProcedureConfig(/*ApexProcedureParameters* p_parameters,*/XERCES_CPP_NAMESPACE::DOMElement* p_base);

        virtual const bool usesFeedback() const { return false; };


};

}

#endif
