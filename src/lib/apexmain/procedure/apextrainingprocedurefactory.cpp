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

#include "apextrainingprocedurefactory.h"
#include "apextrainingprocedure.h"
#include "trainingprocedureparser.h"
#include "apextrainingprocedureparametersparser.h"

#include "xml/xercesinclude.h"
using namespace xercesc;

namespace apex
{

ApexTrainingProcedureFactory::ApexTrainingProcedureFactory()
        : ApexProcedureFactory()
{
}


ApexTrainingProcedureFactory::~ApexTrainingProcedureFactory()
{
}


ApexTrainingProcedure* ApexTrainingProcedureFactory::MakeProcedure(ExperimentRunDelegate& p_rd, ApexProcedureConfig* config)
{
    return new ApexTrainingProcedure(p_rd, config);
}



ApexProcedureConfig * ApexTrainingProcedureFactory::MakeProcedureConfig( DOMElement* p_base )
{
    /*ApexTrainingProcedureParameters* param = MakeProcedureParameters(GetParamElement(p_base));
    return new ApexProcedureConfig(param, p_base);*/

    ApexProcedureConfig* config = new ApexProcedureConfig();
    parser::TrainingProcedureParser parser;
    parser.Parse(p_base,config);
    return config;

}


}
