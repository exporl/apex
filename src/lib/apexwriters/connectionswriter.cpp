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

#include "apextools/xml/xmltools.h"

#include "common/global.h"

#include "connectionswriter.h"

#include <QDebug>

using namespace apex;

using apex::writer::ConnectionsWriter;
using apex::data::ConnectionsData;
using apex::data::ConnectionData;

QDomElement ConnectionsWriter::addElement(QDomDocument *doc,
        const ConnectionsData& data)
{
    QDomElement rootElement = doc->documentElement();
    QDomElement connections = doc->createElement(QSL("connections"));
    rootElement.appendChild(connections);

    ConnectionsData::const_iterator it;
    for (it = data.begin(); it != data.end(); it++)
        connections.appendChild(addConnection(doc, **it));

    return connections;
}

QDomElement ConnectionsWriter::addConnection(QDomDocument *doc,
        const ConnectionData& data)
{
    QDomElement connection = doc->createElement(QSL("connection"));

    //from
    QDomElement from = doc->createElement(QSL("from"));
    from.appendChild(XmlUtils::createTextElement(doc, "id", data.fromId()));
    QDomElement fromCh = XmlUtils::createTextElement(doc, "channel",
                         QString::number(data.fromChannel()));
    QString fromChId = data.fromChannelId();

    if (!fromChId.isEmpty())
        fromCh.setAttribute(QSL("id"), fromChId);

    from.appendChild(fromCh);

    //to
    QDomElement to = doc->createElement(QSL("to"));
    to.appendChild(XmlUtils::createTextElement(doc, "id", data.toId()));
    QDomElement toCh = XmlUtils::createTextElement(doc, "channel",
                       QString::number(data.toChannel()));
    QString toChId = data.toChannelId();

    if (!toChId.isEmpty())
        toCh.setAttribute(QSL("id"), toChId);

    to.appendChild(toCh);

    connection.appendChild(from);
    connection.appendChild(to);

    return connection;
}






























