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

#include "apextools/xml/xmltools.h"

#include "common/global.h"

#include "datablockswriter.h"
#include "fileprefixwriter.h"

using namespace apex;

using apex::data::DatablockData;
using apex::data::DatablocksData;
using apex::writer::DatablocksWriter;

QDomElement DatablocksWriter::addElement(QDomDocument *doc,
                                         const data::DatablocksData &d)
{
    QDomElement rootElem = doc->documentElement();

    QDomElement datablocks = doc->createElement(QSL("datablocks"));
    rootElem.appendChild(datablocks);

    QDomElement prefix = FilePrefixWriter().addElement(doc, d.prefix());

    datablocks.appendChild(prefix);

    for (DatablocksData::const_iterator it = d.begin(); it != d.end(); ++it) {
        QDomElement datablock = doc->createElement(QSL("datablock"));
        datablocks.appendChild(datablock);

        DatablockData *d = it.value();

        datablock.setAttribute(QSL("id"), d->id());

        if (!d->device().isEmpty()) {
            datablock.appendChild(
                XmlUtils::createTextElement(doc, QSL("device"), d->device()));
        }

        if (!d->description().isEmpty()) {
            datablock.appendChild(XmlUtils::createTextElement(
                doc, QSL("description"), d->description()));
        }

        if (!d->file().isEmpty())
            datablock.appendChild(
                XmlUtils::createTextElement(doc, QSL("file"), d->file()));

        if (!d->directData().isEmpty())
            datablock.appendChild(doc->importNode(
                XmlUtils::parseString(d->directData()).documentElement(),
                true));

        if (d->nbChannels())
            datablock.appendChild(XmlUtils::createTextElement(
                doc, QSL("channels"), d->nbChannels()));

        if (d->nbLoops() != 1)
            datablock.appendChild(
                XmlUtils::createTextElement(doc, QSL("loop"), d->nbLoops()));
    }

    if (d.hasPluginDatablocks())
        datablocks.appendChild(doc->createElement(QSL("plugindatablocks")));

    return datablocks;
}
