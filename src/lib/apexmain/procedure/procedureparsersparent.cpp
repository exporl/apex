/******************************************************************************
 * Copyright (C) 2008  Job Noorman <jobnoorman@gmail.com>                     *
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

#include "procedureparsersparent.h"
#include "apextrialfactory.h"

//from libdata
#include "procedure/apexprocedureconfig.h"
#include "procedure/apextrial.h"
#include "parser/scriptexpander.h"

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


ProcedureParsersParent::ProcedureParsersParent(QWidget* parent) :
    currentConfig ( 0 ),
    m_parent(parent)
{
}


void ProcedureParsersParent::Parse ( XERCES_CPP_NAMESPACE::DOMElement* p_base,
                                     data::ApexProcedureConfig* c,
                                     const QString& scriptLibraryFile,
                                     const QVariantMap& scriptParameters)
{
    currentConfig=c;
    if ( !ParseTrials ( p_base, scriptLibraryFile, scriptParameters) )
    {
        throw ApexStringException ( "Could not parse trials" );
    }
    c->SetID ( XMLutils::GetAttribute ( p_base,gc_sID ) );

    DOMNode* parametersNode = XMLutils::GetElementsByTagName ( p_base, "parameters" );
    Q_ASSERT ( parametersNode->getNodeType() == DOMNode::ELEMENT_NODE );
    SetProcedureParameters ( ( DOMElement* ) parametersNode );
    currentConfig=0;

}



bool ProcedureParsersParent::ParseTrials ( DOMElement * p_base, const QString& scriptLibraryFile,
                                           const QVariantMap& scriptParameters)
{
    Q_ASSERT ( currentConfig != NULL );
    // parse trials block
    DOMNode* trialNode = XMLutils::GetElementsByTagName ( p_base, "trials" );
    Q_ASSERT ( trialNode );
    Q_ASSERT ( trialNode->getNodeType() == DOMNode::ELEMENT_NODE );

#ifndef NOSCRIPTEXPAND
    // find plugin datablocks and expand them
    for (DOMNode* currentNode=trialNode->getFirstChild(); currentNode!=0;
            currentNode=currentNode->getNextSibling()) {
        const QString tag( XMLutils::GetTagName( currentNode ) );
        if (tag == "plugintrials") {
            qDebug("Script library: %s", qPrintable(scriptLibraryFile));
            ScriptExpander expander(scriptLibraryFile, scriptParameters, m_parent);
            expander.ExpandScript(currentNode, "getTrials");
        }
    }
#endif

    ApexTrialFactory trialFactory;
    bool success=true;
    // for each trial
    for ( DOMNode* currentNode=trialNode->getFirstChild(); currentNode!=0; currentNode=currentNode->getNextSibling() )
    {
        Q_ASSERT ( currentNode );
        Q_ASSERT ( currentNode->getNodeType() == DOMNode::ELEMENT_NODE );

        const QString tag = XMLutils::GetTagName ( currentNode );
        if (tag == "trial") {
            data::ApexTrial* newTrial = trialFactory.GetTrial ( currentNode );
            Q_ASSERT ( newTrial );
            currentConfig->AddTrial ( newTrial );
        } else {
            qDebug ( "ApexProcedureConfig::ParseTrials: unknown tag %s", qPrintable ( tag ) );
            success=false;
        }

    }

    return success;

}

}   // ns parser
} // ns apex

