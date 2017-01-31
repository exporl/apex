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

#include "apexdata/datablock/datablockdata.h"
#include "apexdata/datablock/datablocksdata.h"

#include "apextools/xml/apexxmltools.h"
#include "apextools/xml/xercesinclude.h"

#include "datablockswriter.h"
#include "fileprefixwriter.h"

#include <QUrl>

using namespace XERCES_CPP_NAMESPACE;
using namespace apex;
using namespace apex::ApexXMLTools;

using apex::data::DatablockData;
using apex::data::DatablocksData;
using apex::writer::DatablocksWriter;

DOMElement* DatablocksWriter::addElement(DOMDocument* doc, const data::DatablocksData& d)
{

    DOMElement* rootElem = doc->getDocumentElement();

    DOMElement*  datablocks = doc->createElement(X("datablocks"));
    rootElem->appendChild(datablocks);

    DOMElement* uri_prefix = FilePrefixWriter().addElement(doc, d.prefix());

    datablocks->appendChild(uri_prefix);

    for (DatablocksData::const_iterator it = d.begin(); it != d.end(); ++it)
    {
        //qCDebug(APEX_RS, "Creating datablock node");

        DOMElement* datablock = doc->createElement(X("datablock"));
        datablocks->appendChild(datablock);

        DatablockData* d = it.value();

        datablock->setAttribute(X("id"), S2X(d->id()));

        if (!d->device().isEmpty())
        {
            datablock->appendChild(
                XMLutils::CreateTextElement(doc, "device" , d->device()));
        }

        if (!d->description().isEmpty())
        {
            datablock->appendChild(
                XMLutils::CreateTextElement(doc, "description" , d->description()));
        }

        if (!d->checksum().isEmpty())
            datablock->appendChild(
                XMLutils::CreateTextElement(doc, "checksum" , d->checksum()));

         if (!d->uri().isEmpty())
            datablock->appendChild(
                        XMLutils::CreateTextElement(doc, "uri" , d->uri().toString().replace(' ', "%20") ));

         if (!d->directData().isEmpty())
            datablock->appendChild(
                        doc->importNode(XMLutils::parseString(d->directData()),
                                        true) );

        if (d->nbChannels())
            datablock->appendChild(
                XMLutils::CreateTextElement(doc, "channels" , d->nbChannels()));

        if (d->nbLoops() != 1)
            datablock->appendChild(
                XMLutils::CreateTextElement(doc, "loop" , d->nbLoops()));


    }



    return datablocks;
}

