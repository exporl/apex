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

#include "apexparametersparser.h"
#include "xml/apexxmltools.h"

#include "xml/xmlkeys.h"

#include "xml/xercesinclude.h"
using namespace XERCES_CPP_NAMESPACE;
using namespace apex::ApexXMLTools;

namespace apex
{

namespace parser
{

ApexParametersParser::ApexParametersParser():
        currentParameters(0)
{
}

void ApexParametersParser::Parse(
    XERCES_CPP_NAMESPACE::DOMElement* p_base,
    data::ApexParameters* p_parameters)
{

    Q_ASSERT (p_base);
    Q_ASSERT(p_parameters);

    currentParameters=p_parameters;


    for (DOMNode* currentNode=p_base->getFirstChild(); currentNode!=0; currentNode=currentNode->getNextSibling())
    {
        Q_ASSERT(currentNode);
        if (currentNode->getNodeType() == DOMNode::ELEMENT_NODE)
        {
            QString tag   = XMLutils::GetTagName( currentNode );
            QString id    = XMLutils::GetAttribute( currentNode, XMLKeys::gc_sID );
            QString value = XMLutils::GetFirstChildText( currentNode );

            bool result = SetParameter(tag, id, value, (DOMElement*) currentNode);

            if (!result)
                qDebug("ApexParametersParser: could not parse tag %s",
                       qPrintable( tag ) );

        }
        else
        {
            Q_ASSERT(0);            // TODO
        }
    }

    currentParameters=0;
}




}

}

