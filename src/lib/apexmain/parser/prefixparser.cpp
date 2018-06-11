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

#include "apextools/xml/xmltools.h"

#include "common/global.h"

#include "prefixparser.h"

namespace apex
{

using data::FilePrefix;

namespace parser
{

PrefixParser::PrefixParser()
{
}

PrefixParser::~PrefixParser()
{
}

/**
 * Parse the given prefix node according to the apex:prefixType:
 * if no attribute is present, the first child text is returned
 * if attribute source="mainconfig", mainconfig is queried for the prefix
 * @param currentNode
 * @return the prefix
*/
FilePrefix PrefixParser::Parse(const QDomElement &currentNode)
{
    FilePrefix prefix;

    const QString attrib(currentNode.attribute(QSL("source")));
    const QString value(currentNode.text());

    prefix.setValue(value.trimmed());
    if (attrib.isEmpty() || attrib == "inline") {
        prefix.setType(FilePrefix::PREFIX_INLINE);
    } else if (attrib == "apexconfig") {
        prefix.setType(FilePrefix::PREFIX_MAINCONFIG);
    } else {
        qFatal("invalid attribute value");
    }
    return prefix;
}
}
}
