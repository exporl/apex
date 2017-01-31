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

#include "adaptiveprocedureparser.h"
#include "apexadaptiveprocedureparametersparser.h"

//from libdata
#include "procedure/apexprocedureconfig.h"
#include "procedure/apexadaptiveprocedureparameters.h"

using namespace apex;
using namespace apex::parser;

AdaptiveProcedureParser::AdaptiveProcedureParser(QWidget* parent) :
    ProcedureParsersParent(parent)
{

}


void AdaptiveProcedureParser::Parse(XERCES_CPP_NAMESPACE::DOMElement* p_base,
           data::ApexProcedureConfig* c, const QString& scriptLibraryFile,
                                    const QVariantMap& scriptParameters)
{
    ProcedureParsersParent::Parse(p_base, c, scriptLibraryFile, scriptParameters);
}





void AdaptiveProcedureParser::SetProcedureParameters(
    XERCES_CPP_NAMESPACE::DOMElement* p_base)
{
    data::ApexAdaptiveProcedureParameters* param = new data::ApexAdaptiveProcedureParameters();
    parser::ApexAdaptiveProcedureParametersParser parser;
    parser.Parse(p_base, param);

    if (! parser.CheckParameters(param))
        throw ApexStringException("Could not parse adaptive procedure parameters");

    currentConfig->SetParameters(param);
}
