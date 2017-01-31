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

#include "fileprefixwriter.h"
#include "fileprefix.h"

#include "xml/apexxmltools.h"
using namespace apex::ApexXMLTools;


#include "xml/xercesinclude.h"
using namespace XERCES_CPP_NAMESPACE;


         using apex::data::FilePrefix;

xercesc::DOMElement* apex::writer::FilePrefixWriter::addElement(
        xercesc::DOMDocument * doc, const data::FilePrefix & d)
{
    DOMElement* result = XMLutils::CreateTextElement(
                             doc,
                             "uri_prefix",
                             d.value());

    if (d.type()==FilePrefix::PREFIX_MAINCONFIG)
        result->setAttribute(X("source"), X("apexconfig"));

    return result;

}
