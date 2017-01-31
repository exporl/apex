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

#include "pluginprocedureparameters.h"
#include <iostream>

#include "xml/xercesinclude.h"
using namespace xercesc;
#include "xml/apexxmltools.h"
using namespace apex::ApexXMLTools;
using namespace apex::data;

#include "apextools.h"

bool tPluginProcedureParameter::operator==(const tPluginProcedureParameter& other) const
{
    return name == other.name && value == other.value;
}

PluginProcedureParameters::PluginProcedureParameters(DOMElement* p_paramElement)
        : ApexProcedureParameters(p_paramElement)
{
    // FIXME: remove
}

PluginProcedureParameters::PluginProcedureParameters()
    : ApexProcedureParameters(),
      m_debugger(1)
{
}


PluginProcedureParameters::~PluginProcedureParameters()
{
}


bool PluginProcedureParameters::SetParameter(const  QString & p_name,const  QString & p_id,const  QString & p_value, DOMElement* d)
{

        if ( p_name == "adjust_parameter") {
            m_adjust_parameter = p_value;
        } else if ( p_name == "script") {
            m_script = p_value;
        } else if ( p_name == "parameter") {
            QString name = XMLutils::GetAttribute(d, "name");
            m_paramList.append( tPluginProcedureParameter(name, p_value));
        } else {

            if ( ApexProcedureParameters::SetParameter(p_name, p_id, p_value,d) == false) {

                qDebug("PluginProcedureParameters::SetParameter: unknown tag %s", qPrintable (p_name));
                return false;
            }
        }

        return true;
}

t_type PluginProcedureParameters::GetType() const
{
	return TYPE_PLUGIN;
}

bool PluginProcedureParameters::operator==(const PluginProcedureParameters& other) const
{
    return  ApexProcedureParameters::operator==(other) &&
            m_script == other.m_script &&
            m_adjust_parameter == other.m_adjust_parameter &&
            ApexTools::haveSameContents(m_paramList, other.m_paramList);
}
