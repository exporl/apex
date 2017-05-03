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

#include "apexdata/procedure/adaptiveproceduredata.h"
#include "apexdata/procedure/proceduredata.h"

#include "apextools/exceptions.h"

#include "apextools/xml/xmltools.h"

#include "adaptiveproceduredataparser.h"
#include "adaptiveprocedureparser.h"

using namespace apex;
using namespace apex::parser;

AdaptiveProcedureParser::AdaptiveProcedureParser()
{
}

data::ProcedureData* AdaptiveProcedureParser::parse(const QDomElement &base)
{
    data::AdaptiveProcedureData* data = new data::AdaptiveProcedureData();
    ProcedureParsersParent::Parse(base, data);
    return data;
}

bool AdaptiveProcedureParser::trialsValid()
{
    return !currentConfig->GetTrials().isEmpty();
}

void AdaptiveProcedureParser::SetProcedureParameters(const QDomElement &p_base)
{
    data::AdaptiveProcedureData* param = dynamic_cast<data::AdaptiveProcedureData*>(currentConfig);
    parser::AdaptiveProcedureDataParser parser;
    parser.Parse(p_base, param);

    if (! parser.CheckParameters(param))
        throw ApexStringException("Could not parse adaptive procedure parameters");

    currentConfig = param;
}
