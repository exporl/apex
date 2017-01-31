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

#include "apexmultiprocedurefactory.h"
#include "apexmultiprocedure.h"
#include "apexmultiprocedureconfig.h"
#include "apexmultiprocedureparameters.h"
#include "apexmultiprocedureparametersparser.h"
#include "multiprocedureparser.h"
#include "apexcontrol.h"
#include "xml/xmlkeys.h"
#include "xml/apexxmltools.h"
using namespace apex::XMLKeys;
using namespace apex::ApexXMLTools;

#include "xml/xercesinclude.h"
using namespace xercesc;

namespace apex {

ApexMultiProcedureFactory::ApexMultiProcedureFactory()
 : ApexProcedureFactory()
{
}


ApexMultiProcedureFactory::~ApexMultiProcedureFactory()
{
}


ApexMultiProcedure* ApexMultiProcedureFactory::MakeProcedure(ExperimentRunDelegate& p_rd, ApexProcedureConfig* p_config)
{
        qDebug("Making ApexMultiProcedure");
        ApexMultiProcedure* proc = new ApexMultiProcedure(p_rd,p_config);
        ApexMultiProcedureConfig* config = (ApexMultiProcedureConfig*) p_config;

        // add subprocedures
        for (tProcConfigList::const_iterator it=config->m_procedures.begin(); it!=config->m_procedures.end(); ++it) {
                ApexProcedureFactory* fact = (*it)->GetProcedureFactory();
                Q_ASSERT(fact);
                ApexProcedure* newproc = fact->MakeProcedure(p_rd,*it);
                proc->AddProcedure(newproc);

                ApexControl::Get().ConnectProcedures(newproc, proc);

        }

        return proc;
}


ApexMultiProcedureConfig * ApexMultiProcedureFactory::MakeProcedureConfig( DOMElement* p_base )
{


    ApexMultiProcedureConfig* config = new ApexMultiProcedureConfig();
    parser::MultiProcedureParser parser;
    parser.Parse(p_base,config);
    return config;


        // order param is already parsed
       /* ApexMultiProcedureParameters* param = MakeProcedureParameters(GetParamElement(p_base)); // own parameters
        Q_CHECK_PTR(param);

        ApexMultiProcedureConfig* config = new ApexMultiProcedureConfig(param, p_base);
        Q_CHECK_PTR(config);

        // parse different procedures and add to config
        for (DOMNode* currentNode=p_base->getFirstChild(); currentNode!=0; currentNode=currentNode->getNextSibling())
        {
                Q_ASSERT(currentNode);
                Q_ASSERT(currentNode->getNodeType() == DOMNode::ELEMENT_NODE);
                const QString tag = XMLutils::GetTagName( currentNode );

                if (tag=="procedure") {
                        const QString type = XMLutils::GetAttribute(currentNode, "xsi:type");
                        const QString id = XMLutils::GetAttribute(currentNode, gc_sID);
                        if (id.isEmpty())
                                throw ApexStringException("Procedure has no ID");
                        ApexProcedureFactory* fact = ApexProcedureFactory::GetProcedureFactory(type);
                        Q_CHECK_PTR(fact);
                        ApexProcedureConfig* newconfig = fact->MakeProcedureConfig((DOMElement*)currentNode);
                        Q_CHECK_PTR(newconfig);
                        newconfig->SetID(id);
                        config->AddProcedureConfig(newconfig);
                }
        }

        qDebug("Making ApexMultiProcedureConfig");
        return config;*/
}


}
