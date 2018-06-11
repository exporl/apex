/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
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

#include "apexdata/procedure/constantproceduredata.h"
#include "apexdata/procedure/proceduredata.h"

#include "constantproceduredataparser.h"
#include "constantprocedureparser.h"

namespace apex
{

namespace parser
{

ConstantProcedureParser::ConstantProcedureParser() : ProcedureParsersParent()
{
}

data::ProcedureData *ConstantProcedureParser::parse(const QDomElement &base)
{
    data::ConstantProcedureData *c = new data::ConstantProcedureData();
    ProcedureParsersParent::Parse(base, c);
    return c;
}

bool ConstantProcedureParser::trialsValid()
{
    return !currentConfig->GetTrials().isEmpty() ||
           currentConfig->hasPluginTrials();
}

void ConstantProcedureParser::SetProcedureParameters(const QDomElement &p_base)
{
    data::ConstantProcedureData *param =
        dynamic_cast<data::ConstantProcedureData *>(currentConfig);
    parser::ConstantProcedureDataParser parser;
    parser.Parse(p_base, param);

    currentConfig = param;
}
}
}
