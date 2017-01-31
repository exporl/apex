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

#include "apextools/xml/xercesinclude.h"

#include "prefixparser.h"

using namespace XERCES_CPP_NAMESPACE;

namespace apex {

    using data::FilePrefix;

namespace parser {

PrefixParser::PrefixParser()
 : Parser()
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

FilePrefix PrefixParser::Parse(XERCES_CPP_NAMESPACE::DOMElement* currentNode) {
    FilePrefix prefix;

    const QString attrib (ApexXMLTools::XMLutils::GetAttribute(currentNode, "source"));
    const QString value (ApexXMLTools::XMLutils::GetFirstChildText (currentNode));

    prefix.setValue( value.trimmed() );
    if (attrib.isEmpty() || attrib == "inline") {
        prefix.setType( FilePrefix::PREFIX_INLINE );
    } else if (attrib == "apexconfig") {
        prefix.setType( FilePrefix::PREFIX_MAINCONFIG );
    } else {
        Q_ASSERT (0 && "invalid attribute value");
    }
    return prefix;

}

}

}
