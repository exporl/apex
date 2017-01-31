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

#include "apextools/xml/apexxmltools.h"
#include "apextools/xml/xercesinclude.h"
#include "apextools/xml/xmlkeys.h"

#include "parser/prefixparser.h"
#include "parser/scriptexpander.h"

#include "datablocksparser.h"

#include <QScopedPointer>
#include <QUrl>

using namespace apex::ApexXMLTools;

using namespace XERCES_CPP_NAMESPACE;

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

/** FIXME: remove document parameter
 */
data::DatablocksData DatablocksParser::Parse
    (XERCES_CPP_NAMESPACE::DOMElement* p_datablocks,
     const QString& scriptLibraryFile, const QVariantMap &scriptParameters)
{
    data::DatablocksData result;

#ifndef NOSCRIPTEXPAND
    // find plugin datablocks and expand them
    for (DOMNode* currentNode=p_datablocks->getFirstChild(); currentNode!=0;
            currentNode=currentNode->getNextSibling()) {
        const QString tag( XMLutils::GetTagName( currentNode ) );
        if (tag == "plugindatablocks") {
            qCDebug(APEX_RS, "Script library: %s", qPrintable(scriptLibraryFile));
            ScriptExpander expander(scriptLibraryFile, scriptParameters,
                                    m_parent);
            expander.ExpandScript(currentNode, "getDatablocks");
        }
    }
#endif

    for (DOMNode* currentNode=p_datablocks->getFirstChild(); currentNode!=0;
            currentNode=currentNode->getNextSibling()) {
        Q_ASSERT(currentNode);

        const QString tag = ApexXMLTools::XMLutils::GetTagName(currentNode);

        //std::cout << "nodetype: " << currentNode->getNodeType() << std::endl;
        Q_ASSERT(currentNode->getNodeType() == DOMNode::ELEMENT_NODE);

        if (tag == "datablock") {
            QScopedPointer<data::DatablockData>
                d(ParseDatablock((DOMElement*)currentNode, result.prefix()));
            if (!d)
                throw ApexStringException("Unknown error creating datablock");
            result[d->id()] = d.data();
            // not done above, otherwise d->id() does not work on Windows
            d.take();
        } else if (tag == "uri_prefix") {
            result.setPrefix(PrefixParser::Parse((DOMElement*)currentNode));
        } else {
            throw ApexStringException( "DatablocksParser::Parse: Unknown "
                    "tag: \"" + tag + "\"" );
        }
    }

    return result;
}

/** FIXME: remove document parameter
*/
data::DatablockData* DatablocksParser::ParseDatablock
        (XERCES_CPP_NAMESPACE::DOMElement* p_datablock,
         data::FilePrefix p_prefix)
{
    //error checks
    if (!p_datablock)
        return 0;

    const QString id = ApexXMLTools::XMLutils::GetAttribute(p_datablock,
            XMLKeys::sc_sID);
    if (id.isEmpty())
        return 0;

    QScopedPointer<data::DatablockData> dummy(new data::DatablockData());
    dummy->setId(id);

    for (DOMNode* currentNode = p_datablock->getFirstChild(); currentNode != 0;
            currentNode = currentNode->getNextSibling())
    {
        const QString tag(ApexXMLTools::XMLutils::GetTagName(currentNode));
        Q_ASSERT(currentNode->getNodeType() == DOMNode::ELEMENT_NODE);

        DOMElement* el = (DOMElement*) currentNode;
        Q_ASSERT(!el->getFirstChild() ||
                 el->getFirstChild()->getNodeType() == DOMNode::TEXT_NODE ||
                 tag == "data");
        QString nodeText;
        if (tag == "data") {
            for (DOMNode* dataNode = el->getFirstChild(); dataNode != NULL;
                    dataNode = dataNode->getNextSibling()) {
                if (dataNode->getNodeType() == DOMNode::ELEMENT_NODE) {
                    nodeText += ApexXMLTools::XMLutils::elementToString
                        (static_cast<DOMElement*>(dataNode));
                } else {
                    // This should take care of text and cdata nodes
                    nodeText += X2S(dataNode->getNodeValue());
                }
            }
        } else {
            nodeText = ApexXMLTools::XMLutils::GetFirstChildText(currentNode);
        }

        if (tag == "device") {
            dummy->setDevice(nodeText);

            //get the module
/*                QString sModule = ApexXMLTools::XMLutils::FindAttribute(
                        p_datablock->getOwnerDocument()->getDocumentElement()
                        ->getFirstChild(), "devices",
                              "device", dummy->m_device, XMLKeys::sc_sType);
            dummy->m_type=sModule;*/
        }
        else if (tag == "description")
            dummy->setDescription(nodeText);
        else if (tag == "checksum") {
            dummy->setChecksum(nodeText);
            QString scheck = ApexXMLTools::XMLutils::GetAttribute(currentNode,
                    "check");
            if (scheck == "false")
                dummy->setDoChecksum(false);
            else
                dummy->setDoChecksum(true);
        } else if (tag == "uri") {
            QUrl theUrl(nodeText);

            /*if (theUrl.isRelative())
            {
                QString modprefix = p_prefix;
                if (!modprefix.isEmpty() && !modprefix.endsWith('/'))
                    modprefix += '/';
                dummy->m_uri = QUrl(modprefix + nodeText);
            }
            else
                dummy->m_uri = QUrl(nodeText);*/
            // FIXME: move this to some convertor
            dummy->setUri( theUrl );
            dummy->setPrefix(p_prefix);
        } else if (tag == "data") {
            dummy->setDirectData(nodeText);
        } else if ( tag == "loop" )
            dummy->setNbLoops(nodeText.toUInt());
        else if ( tag == "gain" )
            dummy->setGain(nodeText.toDouble());
        else if ( tag == "channels" )
            dummy->setNbChannels(nodeText.toUInt());
    }

    return dummy.take();
}

} // namespace parser

} // namespace apex
