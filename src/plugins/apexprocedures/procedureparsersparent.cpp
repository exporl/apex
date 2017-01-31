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

#include "apexdata/procedure/proceduredata.h"
#include "apexdata/procedure/trialdata.h"

#include "apextools/exceptions.h"

#include "apextools/xml/apexxmltools.h"
#include "apextools/xml/xercesinclude.h"
#include "apextools/xml/xmlkeys.h"

#include "procedureparsersparent.h"
#include "trialparser.h"

#include <QObject>

using namespace xercesc;
using namespace apex::XMLKeys;
using namespace apex::ApexXMLTools;


namespace apex
{
namespace parser
{


ProcedureParsersParent::ProcedureParsersParent() : currentConfig ( 0 )
{
}


void ProcedureParsersParent::Parse ( XERCES_CPP_NAMESPACE::DOMElement* p_base,
                              data::ProcedureData* c )
{
    currentConfig=c;

    try{
        ParseTrials ( p_base );
    } catch (const ApexStringException& exception) {
        throw ApexStringException(
                QObject::tr("Could not parse trials: %1").arg(exception.what()));
    }
        c->SetID ( XMLutils::GetAttribute ( p_base,gc_sID ) );

        DOMNode* parametersNode = XMLutils::GetElementsByTagName ( p_base, "parameters" );
        Q_ASSERT ( parametersNode->getNodeType() == DOMNode::ELEMENT_NODE );
        SetProcedureParameters ( ( DOMElement* ) parametersNode );

    currentConfig=0;

}



void ProcedureParsersParent::ParseTrials ( DOMElement * p_base )
{
        Q_ASSERT ( currentConfig != NULL );
        // parse trials block
        DOMNode* trialNode = XMLutils::GetElementsByTagName ( p_base, "trials" );
        Q_ASSERT ( trialNode );
        Q_ASSERT ( trialNode->getNodeType() == DOMNode::ELEMENT_NODE );

    TrialParser trialFactory;
        // for each trial
        for ( DOMNode* currentNode=trialNode->getFirstChild(); currentNode!=0; currentNode=currentNode->getNextSibling() )
        {
                Q_ASSERT ( currentNode );
                Q_ASSERT ( currentNode->getNodeType() == DOMNode::ELEMENT_NODE );

                const QString tag = XMLutils::GetTagName ( currentNode );
                if ( tag == "trial" )
                {
                        data::TrialData* newTrial = trialFactory.GetTrial ( currentNode );
                        Q_ASSERT ( newTrial );
                        currentConfig->AddTrial ( newTrial );
                }
                else
                {
            throw ApexStringException(
                        QObject::tr("ProcedureParsersParent::ParseTrials:"
                                    " unknown tag %1").arg(qPrintable(tag)));
                }

        }

    if (!trialsValid()) {
        throw ApexStringException(
                    QObject::tr("For this procedure, there is an incorrect"
                                " number of trials, or some required"
                                " information is missing from the trials."));
    }
}

}   // ns parser
} // ns apex

