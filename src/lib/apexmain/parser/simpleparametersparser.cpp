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

#include "apexdata/parameters/parametermanagerdata.h"
#include "apexdata/parameters/simpleparameters.h"

#include "apextools/xml/xmlkeys.h"
#include "apextools/xml/xmltools.h"

#include "common/global.h"

#include "simpleparametersparser.h"

#include <QDebug>

namespace apex
{

namespace parser
{

SimpleParametersParser::SimpleParametersParser() : parameterManagerData(0)
{
}

SimpleParametersParser::SimpleParametersParser(data::ParameterManagerData *d)
    : parameterManagerData(d)
{
}

void SimpleParametersParser::Parse(const QDomElement &base,
                                   data::SimpleParameters *p)
{
    QString owner = base.attribute(QSL("id"));
    p->setId(owner);

    QString xsitype = base.attribute(QSL("xsi:type"));
    p->setXsiType(xsitype);

    for (QDomElement currentNode = base.firstChildElement();
         !currentNode.isNull();
         currentNode = currentNode.nextSiblingElement()) {
        QString type = currentNode.tagName();
        QString name = currentNode.attribute(QSL("name"));
        if (type == "parameter" && !name.isEmpty())
            type = name;

        QVariant value(currentNode.text());
        QString id(currentNode.attribute(QSL("id")));
        int channel;
        QString sChannel(currentNode.attribute(QSL("channel")));

        if (sChannel.isEmpty())
            channel = -1;
        else
            channel = sChannel.toInt();

        AddParameter(p, currentNode, owner, type, id, value, channel);
    }

    return;
}

void SimpleParametersParser::AddParameter(
    data::SimpleParameters *p, const QDomElement &e, const QString &owner,
    const QString &type, const QString &id, const QVariant &value,
    const int channel)
{
    Q_UNUSED(e);
    data::Parameter t(owner, type, value, channel, !id.isEmpty(), id);
    p->addParameter(t);

    if (parameterManagerData)
        parameterManagerData->registerParameter(id, t);
}

} // ns parser

} // ns apex
