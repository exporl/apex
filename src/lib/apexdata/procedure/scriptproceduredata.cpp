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

#include "apextools/apextools.h"

#include "apextools/xml/apexxmltools.h"
#include "apextools/xml/xercesinclude.h"

#include "scriptproceduredata.h"

#include <iostream>

using namespace xercesc;
using namespace apex::ApexXMLTools;
using namespace apex::data;

tScriptProcedureParameter::tScriptProcedureParameter(const QString& p_name,
                                                     const QString& p_value):
        name(p_name),
        value(p_value)
{
}

bool tScriptProcedureParameter::operator==(const tScriptProcedureParameter& other) const
{
    return name == other.name && value == other.value;
}

ScriptProcedureData::ScriptProcedureData()
{
}


ScriptProcedureData::~ScriptProcedureData()
{
}

ScriptProcedureData::Type ScriptProcedureData::type() const
{
    return PluginType;
}

bool ScriptProcedureData::operator==(const ScriptProcedureData& other) const
{
    return  m_script == other.m_script &&
            m_adjust_parameter == other.m_adjust_parameter &&
            ApexTools::haveSameContents(m_paramList, other.m_paramList);
}

QString ScriptProcedureData::name() const
{
     return QLatin1String("apex:pluginProcedure");
}

const QString ScriptProcedureData::script() const
{
    return m_script;
}

void ScriptProcedureData::setScript(const QString& p)
{
    m_script = p;
}

const QString ScriptProcedureData::adjustParameter() const
{
    return m_adjust_parameter;
}

void ScriptProcedureData::setAdjustParameter(const QString& p)
{
    m_adjust_parameter = p;
}

bool ScriptProcedureData::debugger() const
{
    return m_debugger;
}

void ScriptProcedureData::setDebugger(bool p)
{
    m_debugger = p;
}

const tScriptProcedureParameterList& ScriptProcedureData::customParameters() const
{
    return m_paramList;
}

void ScriptProcedureData::appendParameter(const QString& name, const QString& value) {
    m_paramList.append(data::tScriptProcedureParameter(name, value));
}

