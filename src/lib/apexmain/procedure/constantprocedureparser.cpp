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

#include "constantprocedureparser.h"
#include "apexconstantprocedureparametersparser.h"

//from libdata
#include "procedure/apexconstantprocedureparameters.h"
#include "procedure/apexprocedureconfig.h"



namespace apex
{

namespace parser
{

ConstantProcedureParser::ConstantProcedureParser(QWidget* parent) :
    ProcedureParsersParent(parent)
{

}


void ConstantProcedureParser::Parse(XERCES_CPP_NAMESPACE::DOMElement* p_base,
           data::ApexProcedureConfig* c, const QString& scriptLibraryFile)
{
    ProcedureParsersParent::Parse(p_base, c, scriptLibraryFile);
}





void ConstantProcedureParser::SetProcedureParameters(
    XERCES_CPP_NAMESPACE::DOMElement* p_base)
{
    data::ApexConstantProcedureParameters* param = new data::ApexConstantProcedureParameters();
    parser::ApexConstantProcedureParametersParser parser;
    parser.Parse(p_base, param);

    currentConfig->SetParameters(param);
}

}
}
