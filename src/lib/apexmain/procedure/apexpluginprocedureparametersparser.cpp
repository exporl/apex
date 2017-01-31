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

#include "apexpluginprocedureparametersparser.h"
#include "procedure/pluginprocedureparameters.h"

#include "xml/xercesinclude.h"
namespace XERCES_CPP_NAMESPACE
{
    class DOMElement;
};
using namespace apex::ApexXMLTools;


#include <QString>

namespace apex
{

namespace parser
{


ApexPluginProcedureParametersParser::ApexPluginProcedureParametersParser()
{
}

void ApexPluginProcedureParametersParser::Parse(
        XERCES_CPP_NAMESPACE::DOMElement* p_base,
        data::PluginProcedureParameters* p_parameters)
{
   ApexProcedureParametersParser::Parse(p_base, p_parameters);
}

bool ApexPluginProcedureParametersParser::SetParameter(const QString p_name,
        const QString id, const QString p_value ,
        XERCES_CPP_NAMESPACE::DOMElement* node)
{
    Q_ASSERT(currentParameters);

    data::PluginProcedureParameters* param =
            dynamic_cast<data::PluginProcedureParameters*>(currentParameters);

    Q_ASSERT (param);

    if ( p_name == "adjust_parameter") {
        param->m_adjust_parameter = p_value;
    } else if ( p_name == "script") {
        param->m_script = p_value;
    } else if ( p_name == "parameter") {
        QString name = XMLutils::GetAttribute(node, "name");
        param->m_paramList.append( data::tPluginProcedureParameter(name, p_value));
    } else {
        return
                ApexProcedureParametersParser::SetParameter(
                p_name,id, p_value, node);

    }

    return true;





}


}

}


