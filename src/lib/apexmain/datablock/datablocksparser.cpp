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

#include "apextools/exceptions.h"

#include "apextools/xml/xmlkeys.h"
#include "apextools/xml/xmltools.h"

#include "common/global.h"

#include "parser/prefixparser.h"
#include "parser/scriptexpander.h"

#include "datablocksparser.h"

#include <QScopedPointer>

using apex::data::FilePrefix;

namespace apex
{

namespace parser
{

DatablocksParser::DatablocksParser(QWidget* parent) :
    m_parent(parent)
{
}

DatablocksParser::~DatablocksParser()
{
}

data::DatablocksData DatablocksParser::Parse(
    const QString &fileName, const QDomElement &p_datablocks,
    const QString &scriptLibraryFile, const QVariantMap &scriptParameters)
{
    data::DatablocksData result;

#ifndef NOSCRIPTEXPAND
    // find plugin datablocks and expand them
    for (QDomElement currentNode = p_datablocks.firstChildElement(); !currentNode.isNull();
            currentNode = currentNode.nextSiblingElement()) {
        const QString tag(currentNode.tagName());
        if (tag == "plugindatablocks") {
            qCDebug(APEX_RS, "Script library: %s", qPrintable(scriptLibraryFile));
            ScriptExpander expander(fileName, scriptLibraryFile, scriptParameters,
                                    m_parent);
            expander.ExpandScript(currentNode, "getDatablocks");
        }
    }
#endif

    for (QDomElement currentNode = p_datablocks.firstChildElement(); !currentNode.isNull();
            currentNode = currentNode.nextSiblingElement()) {
        const QString tag = currentNode.tagName();
        if (tag == "datablock") {
            QScopedPointer<data::DatablockData> d(ParseDatablock(currentNode, result.prefix()));
            result[d->id()] = d.data();
            // not done above, otherwise d->id() does not work
            d.take();
        } else if (tag == "prefix") {
            result.setPrefix(PrefixParser::Parse(currentNode));
        } else {
            throw ApexStringException( "DatablocksParser::Parse: Unknown "
                    "tag: \"" + tag + "\"" );
        }
    }

    return result;
}

data::DatablockData* DatablocksParser::ParseDatablock(const QDomElement &p_datablock,
        data::FilePrefix p_prefix)
{
    QScopedPointer<data::DatablockData> dummy(new data::DatablockData());
    dummy->setId(p_datablock.attribute(XMLKeys::sc_sID));

    for (QDomElement currentNode = p_datablock.firstChildElement(); !currentNode.isNull();
            currentNode = currentNode.nextSiblingElement()) {
        const QString tag(currentNode.tagName());
        QString nodeText;
        if (tag == "data") {
            for (QDomNode dataNode = currentNode.firstChild(); !dataNode.isNull();
                    dataNode = dataNode.nextSibling()) {
                if (dataNode.isElement()) {
                    nodeText += XmlUtils::nodeToString(dataNode);
                } else {
                    // This should take care of text and cdata nodes
                    nodeText += dataNode.nodeValue();
                }
            }
        } else {
            nodeText = currentNode.text();
        }

        if (tag == "device") {
            dummy->setDevice(nodeText);
        } else if (tag == "description") {
            dummy->setDescription(nodeText);
        } else if (tag == "file") {
            dummy->setFile(nodeText);
            dummy->setPrefix(p_prefix);
        } else if (tag == "data") {
            dummy->setDirectData(nodeText);
        } else if ( tag == "loop" ) {
            dummy->setNbLoops(nodeText.toUInt());
        } else if ( tag == "gain" ) {
            dummy->setGain(nodeText.toDouble());
        } else if ( tag == "channels") {
            dummy->setNbChannels(nodeText.toUInt());
        }
    }

    return dummy.take();
}

}
}
