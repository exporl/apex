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

#include "apexdata/corrector/correctordata.h"

#include "apextools/xml/xmltools.h"

#include "common/global.h"

#include "correctorparser.h"

using namespace apex;
using data::CorrectorData;

CorrectorData *CorrectorParser::Parse(const QDomElement &dom)
{
    QScopedPointer<CorrectorData> data(new CorrectorData);
    QString type = dom.attribute(QSL("xsi:type"));

    if (type == "apex:isequal")
        data->setType(CorrectorData::EQUAL);
    else
        qFatal("Invalid corrector type %s", qPrintable(type));

    for (QDomElement currentNode = dom.firstChildElement();
         !currentNode.isNull();
         currentNode = currentNode.nextSiblingElement()) {
        const QString tag = currentNode.tagName();
        if (tag == "language") {
            const QString value = currentNode.text();
            if (value == "Dutch")
                data->setLanguage(CorrectorData::DUTCH);
            else if (value == "English")
                data->setLanguage(CorrectorData::ENGLISH);
        } else {
            qFatal("Unknown corrector tag: %s", qPrintable(tag));
        }
    }
    return data.take();
}
