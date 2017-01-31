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

#include "apexadaptiveprocedurefactory.h"
#include "apexadaptiveprocedure.h"
#include "adaptiveprocedureparser.h"
#include "apexadaptiveprocedureparametersparser.h"

#include "xml/xercesinclude.h"
using namespace xercesc;

namespace apex
{

ApexAdaptiveProcedureFactory::ApexAdaptiveProcedureFactory()
        : ApexProcedureFactory()
{
}


ApexAdaptiveProcedureFactory::~ApexAdaptiveProcedureFactory()
{
}


ApexAdaptiveProcedure* ApexAdaptiveProcedureFactory::MakeProcedure(ExperimentRunDelegate& p_rd, ApexProcedureConfig* config )
{
    return new ApexAdaptiveProcedure(p_rd, config);
}




ApexProcedureConfig * ApexAdaptiveProcedureFactory::MakeProcedureConfig(DOMElement* p_base )
{
    /*ApexAdaptiveProcedureParameters *param = MakeProcedureParameters(GetParamElement(p_base));
    return new ApexProcedureConfig(param,p_base,this);*/

    ApexProcedureConfig* config = new ApexProcedureConfig();
    parser::AdaptiveProcedureParser parser;
    parser.Parse(p_base,config);
    return config;
}

}
