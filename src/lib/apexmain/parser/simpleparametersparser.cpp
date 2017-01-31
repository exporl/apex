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

#include "simpleparametersparser.h"
#include "parameters/simpleparameters.h"
#include "parameters/parametermanagerdata.h"

#include "xml/apexxmltools.h"
#include "xml/xmlkeys.h"
using namespace apex::ApexXMLTools;

#include "xml/xercesinclude.h"
using namespace XERCES_CPP_NAMESPACE;

#include <QDebug>


namespace apex
{

namespace parser
{

SimpleParametersParser::SimpleParametersParser() : parameterManagerData(0)
{
}

SimpleParametersParser::SimpleParametersParser(data::ParameterManagerData* d) :
                                                        parameterManagerData(d)
{
}

void SimpleParametersParser::Parse(XERCES_CPP_NAMESPACE::DOMElement* base,
                                   data::SimpleParameters* p)
{
    QString owner = ApexXMLTools::XMLutils::GetAttribute(base, "id");
    p->setId(owner);

    QString xsitype = ApexXMLTools::XMLutils::GetAttribute(base, "xsi:type");
    p->setXsiType(xsitype);

    for (DOMNode* currentNode = base->getFirstChild(); currentNode != 0;
         currentNode=currentNode->getNextSibling())
    {
        Q_ASSERT(currentNode);
        Q_ASSERT(currentNode->getNodeType() == DOMNode::ELEMENT_NODE);

        QString type = ApexXMLTools::XMLutils::GetTagName( currentNode );
        QString name = ApexXMLTools::XMLutils::GetAttribute(currentNode, "name");
        if (type=="parameter" && !name.isEmpty())
            type=name;

        QVariant value( ApexXMLTools::XMLutils::GetFirstChildText( currentNode ) );
        QString id( ApexXMLTools::XMLutils::GetAttribute(currentNode, "id"));
        int channel;
        QString sChannel(ApexXMLTools::XMLutils::GetAttribute(currentNode, "channel"));

        if (sChannel.isEmpty())
            channel=-1;
        else
            channel=sChannel.toInt();


        //qDebug("Adding parameter %s with value %s to %s", qPrintable(type), qPrintable(value.toString()), qPrintable(id));
        AddParameter(p, (DOMElement*) currentNode, owner, type, id, value, channel);

    }

    //[job setDefault] p->setDefaultParameterValues();

    return;
}



void SimpleParametersParser::AddParameter(data::SimpleParameters* p,
                                          XERCES_CPP_NAMESPACE::DOMElement* e,
                                          const QString& owner,
                                          const QString& type,
                                          const QString& id,
                                          const QVariant& value,
                                          const int channel)
{
    Q_UNUSED (e);
    data::Parameter t(owner, type, value, channel,!id.isEmpty(), id);
    p->addParameter(t);

    if (parameterManagerData)
        parameterManagerData->registerParameter(id, t);
}


} // ns parser

} // ns apex
