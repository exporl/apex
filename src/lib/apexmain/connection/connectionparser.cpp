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

#include "apexdata/connection/connectiondata.h"

#include "apexdata/parameters/parametermanagerdata.h"

#include "apextools/xml/apexxmltools.h"
#include "apextools/xml/xercesinclude.h"
#include "apextools/xml/xmlkeys.h"

#include "connectionparser.h"

#include <memory>

using namespace xercesc;
using namespace apex;
using namespace apex::data;
using namespace apex::XMLKeys;
using namespace apex::ApexXMLTools;
using namespace apex::parser;

ConnectionData* ConnectionParser::Parse (xercesc::DOMNode* node)
{
    QScopedPointer<ConnectionData> data(new ConnectionData());

    for (DOMNode* connNode = node->getFirstChild(); connNode != NULL; connNode = connNode->getNextSibling()) {
        Q_ASSERT (connNode->getNodeType() == DOMNode::ELEMENT_NODE);
        const QString tag (XMLutils::GetTagName (connNode));

        if (tag == "from") {
            for (DOMNode* fromNode = connNode->getFirstChild(); fromNode != 0; fromNode = fromNode->getNextSibling()) {
                const QString fromTag (XMLutils::GetTagName (fromNode));
                if (fromTag == gc_sID) {
                    data->setFromId (XMLutils::GetFirstChildText (fromNode));
                    QString matchType =
                            XMLutils::GetAttribute(fromNode, "mode");
                    if (matchType == "wildcard")
                        data->setMatchType(MATCH_WILDCARD);
                    else if (matchType == "name")
                        data->setMatchType(MATCH_NAME);
                    else if (matchType == "regexp")
                        data->setMatchType(MATCH_REGEXP);
                    else if (! matchType.isEmpty())
                        qFatal("Invalid match type");
                } else if (fromTag == gc_sChannel)
                    data->setFromChannel (XMLutils::GetFirstChildText (fromNode).toInt(),
                        XMLutils::GetAttribute (fromNode, gc_sID));
            }
        } else if (tag == "to") {
            for (DOMNode* toNode = connNode->getFirstChild(); toNode != 0; toNode = toNode->getNextSibling()) {
                const QString toTag (XMLutils::GetTagName (toNode));
                if (toTag == gc_sID)
                    data->setToId (XMLutils::GetFirstChildText (toNode));
                else if (toTag == gc_sChannel) {
                    QString chanid(XMLutils::GetAttribute (toNode, gc_sID));
                    int value = XMLutils::GetFirstChildText (toNode).toInt();
                    data->setToChannel (value, chanid);
                }
            }
        } else
            qFatal ("Unknown node name in connection: %s", qPrintable (tag));
    }



    return data.take();
}


