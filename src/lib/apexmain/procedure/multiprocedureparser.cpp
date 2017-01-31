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

#include "multiprocedureparser.h"
#include "apexmultiprocedureparametersparser.h"
//#include "apexprocedurefactory.h"
#include "procedureparser.h"

//from libdata
#include "procedure/apexmultiprocedureparameters.h"
#include "procedure/apexmultiprocedureconfig.h"

#include "xml/xercesinclude.h"
using namespace xercesc;
#include "xml/xmlkeys.h"
#include "xml/apexxmltools.h"
using namespace apex::XMLKeys;
using namespace apex::ApexXMLTools;
#include "exceptions.h"

namespace apex
{

namespace parser
{

MultiProcedureParser::MultiProcedureParser(QWidget* parent) :
    ProcedureParsersParent(parent)
{
}


void MultiProcedureParser::Parse(XERCES_CPP_NAMESPACE::DOMElement* p_base,
                                 data::ApexMultiProcedureConfig* config,
                                 const QString& scriptLibraryFile,
                                 const QVariantMap& scriptParameters)
{
    currentConfig=config;
    config->SetID( XMLutils::GetAttribute(p_base,gc_sID) );


    Q_CHECK_PTR(config);

    // parse different procedures and add to config
    for (DOMNode* currentNode=p_base->getFirstChild(); currentNode!=0; currentNode=currentNode->getNextSibling())
    {
        Q_ASSERT(currentNode);
        Q_ASSERT(currentNode->getNodeType() == DOMNode::ELEMENT_NODE);
        const QString tag = XMLutils::GetTagName( currentNode );

        if (tag=="parameters")
        {
            SetProcedureParameters((DOMElement*)currentNode);
        }
        else if (tag=="procedure")
        {
            const QString type = XMLutils::GetAttribute(currentNode, "xsi:type");
            const QString id = XMLutils::GetAttribute(currentNode, gc_sID);
            if (id.isEmpty())
                throw ApexStringException("Procedure has no ID");

            //ApexProcedureFactory* fact = ApexProcedureFactory::GetProcedureFactory(type);
            ProcedureParser parser(m_parent);
            //Q_CHECK_PTR(fact);
            //ApexProcedureConfig* newconfig = fact->MakeProcedureConfig((DOMElement*)currentNode);
            data::ApexProcedureConfig* newconfig = parser.Parse( (DOMElement*)currentNode,
                                                                 scriptLibraryFile, scriptParameters);
            Q_CHECK_PTR(newconfig);
            newconfig->SetID(id);
            //newconfig->SetProcedureFactory(fact);
            config->AddProcedureConfig(newconfig);
            qDebug("Adding procedureconfig to multiprocedure with pointer %p",
                   newconfig);
        }
    }

    currentConfig=0;
}





void MultiProcedureParser::SetProcedureParameters(
    XERCES_CPP_NAMESPACE::DOMElement* p_base)
{
    data::ApexMultiProcedureParameters* param = new data::ApexMultiProcedureParameters();
    parser::ApexMultiProcedureParametersParser parser;
    parser.Parse(p_base, param);

    currentConfig->SetParameters(param);
}

}
}
