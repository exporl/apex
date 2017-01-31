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

#include "apextools/xml/apexxmltools.h"
#include "apextools/xml/xercesinclude.h"
#include "apextools/xml/xmlkeys.h"

#include "trialparser.h"

#include <QString>

#include <iostream>

using namespace apex::XMLKeys;
using namespace apex::ApexXMLTools;

using namespace xercesc;


namespace apex {

TrialParser::TrialParser()
{
}


TrialParser::~TrialParser()
{
}



data::TrialData* TrialParser::GetTrial( DOMNode * p_base )
{
        Q_ASSERT(p_base);
        data::TrialData* newTrial = MakeTrial();
        Q_ASSERT(newTrial);

        // get id
        const QString id = XMLutils::GetAttribute( p_base, gc_sID );
        Q_ASSERT(id.isEmpty()==0);
        newTrial->SetID(id);

        for (DOMNode* currentNode=p_base->getFirstChild(); currentNode!=0; currentNode=currentNode->getNextSibling()) {
                Q_ASSERT(currentNode);
                Q_ASSERT(currentNode->getNodeType() == DOMNode::ELEMENT_NODE);

                const QString tag       = XMLutils::GetTagName( currentNode );
                const QString subid     = XMLutils::GetAttribute( currentNode, gc_sID );
                const QString nodeText  = XMLutils::GetFirstChildText( currentNode );

                if (tag == "answer")  {
                        QString text = GetAnswer(currentNode);
                        newTrial->SetAnswer(text);
                        //Q_ASSERT(nodeText.isEmpty()==0);
                        //newTrial->SetAnswer(nodeText);
                } else if (tag == "answer_element") {
                        Q_ASSERT(nodeText.isEmpty()==0);
                        newTrial->SetAnswerElement(nodeText);
                } else if (tag == "screen") {
                        Q_ASSERT(subid.isEmpty()==0);
                        newTrial->SetScreen(subid);
                } else if (tag == "stimulus") {
                        Q_ASSERT(subid.isEmpty()==0);
                        newTrial->AddStimulus(subid);
                } else if (tag == "standard") {
                        Q_ASSERT(subid.isEmpty()==0);
                        newTrial->AddStandard(subid);
                } else {
                        std::cerr <<"Unknown tag in parsedatablocks: " << qPrintable(tag) << std::endl;
                }

        }

        return newTrial;
}

QString TrialParser::GetAnswer(DOMNode* p_base) {
        return XMLutils::NodeToString(p_base) ;
}

data::TrialData * apex::TrialParser::MakeTrial( )
{
        return new data::TrialData();
}


}
