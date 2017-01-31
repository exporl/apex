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

#include "apexdata/procedure/scriptproceduredata.h"

#include "apextools/xml/apexxmltools.h"
#include "apextools/xml/xercesinclude.h"

#include "scriptproceduredataparser.h"

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


ScriptProcedureDataParser::ScriptProcedureDataParser()
{
}

void ScriptProcedureDataParser::Parse(
        XERCES_CPP_NAMESPACE::DOMElement* p_base,
        data::ScriptProcedureData *p_data)
{
   ProcedureDataParser::Parse(p_base, p_data);
}

bool ScriptProcedureDataParser::SetParameter(const QString p_name,
        const QString id, const QString p_value ,
        XERCES_CPP_NAMESPACE::DOMElement* node, data::ProcedureData *data)
{
    Q_ASSERT(data);

    data::ScriptProcedureData* scriptData =
            dynamic_cast<data::ScriptProcedureData*>(data);

    Q_ASSERT (scriptData);

    if ( p_name == "adjust_parameter") {
        scriptData->setAdjustParameter(p_value);
    } else if ( p_name == "script") {
        scriptData->setScript(p_value);
    } else if ( p_name == "parameter") {
        QString name = XMLutils::GetAttribute(node, "name");
        scriptData->appendParameter(name, p_value);
    } else {
        return
                ProcedureDataParser::SetParameter(
                p_name,id, p_value, node, scriptData);

    }

    return true;





}


}

}


