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

#include "apexdata/connection/connectiondata.h"

#include "apextools/xml/apexxmltools.h"
#include "apextools/xml/xercesinclude.h"

#include "connectionswriter.h"

#include <QDebug>

using namespace XERCES_CPP_NAMESPACE;
using namespace apex::ApexXMLTools;

using apex::writer::ConnectionsWriter;
using apex::data::ConnectionsData;
using apex::data::ConnectionData;

DOMElement* ConnectionsWriter::addElement(DOMDocument* doc,
        const ConnectionsData& data)
{
    DOMElement* rootElement = doc->getDocumentElement();
    DOMElement* connections = doc->createElement(X("connections"));
    rootElement->appendChild(connections);

    ConnectionsData::const_iterator it;
    for (it = data.begin(); it != data.end(); it++)
        connections->appendChild(addConnection(doc, **it));

    return connections;
}

DOMElement* ConnectionsWriter::addConnection(DOMDocument* doc,
        const ConnectionData& data)
{
    DOMElement* connection = doc->createElement(X("connection"));

    //from
    DOMElement* from = doc->createElement(X("from"));
    from->appendChild(XMLutils::CreateTextElement(doc, "id", data.fromId()));
    DOMElement* fromCh = XMLutils::CreateTextElement(doc, "channel",
                         QString::number(data.fromChannel()));
    QString fromChId = data.fromChannelId();

    if (!fromChId.isEmpty())
        fromCh->setAttribute(X("id"), S2X(fromChId));

    from->appendChild(fromCh);

    //to
    DOMElement* to = doc->createElement(X("to"));
    to->appendChild(XMLutils::CreateTextElement(doc, "id", data.toId()));
    DOMElement* toCh = XMLutils::CreateTextElement(doc, "channel",
                       QString::number(data.toChannel()));
    QString toChId = data.toChannelId();

    if (!toChId.isEmpty())
        toCh->setAttribute(X("id"), S2X(toChId));

    to->appendChild(toCh);

    connection->appendChild(from);
    connection->appendChild(to);

    return connection;
}






























