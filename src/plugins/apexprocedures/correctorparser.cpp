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

#include "apexdata/corrector/correctordata.h"

#include "apextools/xml/apexxmltools.h"
#include "apextools/xml/xercesinclude.h"

#include "correctorparser.h"

#include <iostream>

using namespace apex::ApexXMLTools;
using namespace apex;
using namespace xercesc;
using data::CorrectorData;

CorrectorData *CorrectorParser::Parse (DOMElement* dom)
{
    QScopedPointer<CorrectorData> data(new CorrectorData);
    QString type = XMLutils::GetAttribute (dom, "xsi:type");

    if (type == "apex:isequal")
        data->setType(CorrectorData::EQUAL);
    else
        qFatal ("Invalid corrector type %s", qPrintable (type));

    for (DOMNode* currentNode = dom->getFirstChild(); currentNode != NULL;
        currentNode = currentNode->getNextSibling()) {
        Q_ASSERT (currentNode->getNodeType() == DOMNode::ELEMENT_NODE);

        const QString tag = XMLutils::GetTagName (currentNode);
        if (tag == "language") {
            const QString value = XMLutils::GetFirstChildText (currentNode);
            if (value == "Dutch")
                data->setLanguage(CorrectorData::DUTCH);
            else if (value == "English")
                data->setLanguage(CorrectorData::ENGLISH);
        } else
            qFatal ("Unknown corrector tag: %s", qPrintable (tag));
    }
    return data.take();
}
