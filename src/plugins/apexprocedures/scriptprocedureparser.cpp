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

#include "apexdata/procedure/proceduredata.h"
#include "apexdata/procedure/scriptproceduredata.h"

#include "scriptproceduredataparser.h"
#include "scriptprocedureparser.h"

namespace apex
{

namespace parser
{

ScriptProcedureParser::ScriptProcedureParser()
{
}

data::ProcedureData *ScriptProcedureParser::parse(const QDomElement &base)
{
    data::ScriptProcedureData *data = new data::ScriptProcedureData();
    ProcedureParsersParent::Parse(base, data);
    return data;
}

bool ScriptProcedureParser::trialsValid()
{
    return true;
}

void ScriptProcedureParser::SetProcedureParameters(const QDomElement &p_base)
{
    data::ScriptProcedureData *param =
        dynamic_cast<data::ScriptProcedureData *>(currentConfig);
    parser::ScriptProcedureDataParser parser;
    parser.Parse(p_base, param);

    currentConfig = param;
}
}
}
