/******************************************************************************
 * Copyright (C) 2014  Jonas Vanthornhout <jonasvanthornhout+apex@gmail.com>  *
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
#include "apexpaths.h"

#include "version.h"
#include "xpathprocessor.h"

#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include <QFile>

#include <QtGlobal>

namespace apex {

XPathProcessor::XPathProcessor(QString filename)
{
    xmlInitParser();
    LIBXML_TEST_VERSION

    doc = xmlParseFile(QFile::encodeName(filename));
    if(doc == NULL) {
        throw XPathException("Unknown filename", QObject::tr("No file with the name %1 is known.").arg(filename));
    }
}

XPathProcessor::~XPathProcessor()
{
    xmlFreeDoc(doc);
    xmlCleanupParser();
}

bool XPathProcessor::transform(QString xpath, QString newValue)
{
    if(!xpath.startsWith("/")) {
        xpath.prepend("/");
    }

    xmlNodeSetPtr nodes = getNodes(doc, xpath);
    unsigned long size = (nodes) ? nodes->nodeNr : 0;

    if(size == 0) {
        throw XPathException(QObject::tr("Invalid query"),
                             QObject::tr("Warning: your query \"%1\" did not return any results, "
                                         "therefore the requested new value will be ignored.").arg(xpath));
    }

    for(int i=size-1; i>=0; --i) {
        changeNode(nodes->nodeTab[i], newValue);
    }

    return true;
}

xmlNodeSetPtr XPathProcessor::getNodes(xmlDocPtr doc, QString xpath)
{
    const xmlChar* xpathExpr = xmlCharStrdup(xpath.toStdString().c_str());

    xmlXPathContextPtr xpathContext = xmlXPathNewContext(doc);
    if(xpathContext == NULL)
        throw XPathException("XPath error", QObject::tr("Could not create a new XPath context."));

    registerNamespaces(xpathContext, "apex", APEX_NAMESPACE);

    xmlXPathObjectPtr xpathObject = xmlXPathEvalExpression(xpathExpr, xpathContext);
    if(xpathObject == NULL) {
        throw XPathException("XPath error", QObject::tr("Could not execute XPath query %1.").arg(xpath));
        xmlXPathFreeContext(xpathContext);
    }

    return xpathObject->nodesetval;
}

void XPathProcessor::registerNamespaces(xmlXPathContextPtr xpathContext, QString prefix, QString href)
{
    const xmlChar* xmlPrefix = xmlCharStrdup(prefix.toStdString().c_str());
    const xmlChar* xmlHref = xmlCharStrdup(href.toStdString().c_str());

    Q_ASSERT(xpathContext);

    if(xmlXPathRegisterNs(xpathContext, xmlPrefix, xmlHref) != 0)
        throw XPathException("Namespace error", QObject::tr("Could not register namespace %1 with href %2.").arg(prefix).arg(href));
}


void XPathProcessor::changeNode(xmlNodePtr node, QString newValue)
{
    const xmlChar* xmlNewValue = xmlCharStrdup(newValue.toStdString().c_str());

    xmlNodeSetContent(node, xmlNewValue);
}

void XPathProcessor::save(QString filename)
{
    xmlSaveFile(QFile::encodeName(filename), doc);
}

}
