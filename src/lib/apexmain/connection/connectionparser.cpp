/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "apexdata/connection/connectiondata.h"

#include "apexdata/parameters/parametermanagerdata.h"

#include "apextools/xml/xmlkeys.h"
#include "apextools/xml/xmltools.h"

#include "common/global.h"

#include "connectionparser.h"

#include <memory>

using namespace apex;
using namespace apex::data;
using namespace apex::XMLKeys;
using namespace apex::parser;

ConnectionData *ConnectionParser::Parse(const QDomElement &node)
{
    QScopedPointer<ConnectionData> data(new ConnectionData());

    for (QDomElement connNode = node.firstChildElement(); !connNode.isNull();
         connNode = connNode.nextSiblingElement()) {
        const QString tag(connNode.tagName());

        if (tag == "from") {
            for (QDomElement fromNode = connNode.firstChildElement();
                 !fromNode.isNull(); fromNode = fromNode.nextSiblingElement()) {
                const QString fromTag(fromNode.tagName());
                if (fromTag == gc_sID) {
                    data->setFromId(fromNode.text());
                    QString matchType = fromNode.attribute(QSL("mode"));
                    if (matchType == "wildcard")
                        data->setMatchType(MATCH_WILDCARD);
                    else if (matchType == "name")
                        data->setMatchType(MATCH_NAME);
                    else if (matchType == "regexp")
                        data->setMatchType(MATCH_REGEXP);
                    else if (!matchType.isEmpty())
                        qFatal("Invalid match type");
                } else if (fromTag == gc_sChannel) {
                    data->setFromChannel(fromNode.text().toInt(),
                                         fromNode.attribute(gc_sID));
                }
            }
        } else if (tag == "to") {
            for (QDomElement toNode = connNode.firstChildElement();
                 !toNode.isNull(); toNode = toNode.nextSiblingElement()) {
                const QString toTag(toNode.tagName());
                if (toTag == gc_sID)
                    data->setToId(toNode.text());
                else if (toTag == gc_sChannel) {
                    data->setToChannel(toNode.text().toInt(),
                                       toNode.attribute(gc_sID));
                }
            }
        } else
            qFatal("Unknown node name in connection: %s", qPrintable(tag));
    }

    return data.take();
}
