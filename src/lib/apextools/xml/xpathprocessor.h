/******************************************************************************
 * Copyright (C) 2014  Jonas Vanthornhout <jonasvanthornhout+apex@gmail.com>  *
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
#ifndef _EXPORL_SRC_LIB_APEXTOOLS_XML_XPATHPROCESSOR_H_
#define _EXPORL_SRC_LIB_APEXTOOLS_XML_XPATHPROCESSOR_H_

#include "../global.h"

#include <libxml/xmlmemory.h>
#include <libxml/xpath.h>

#include <stdexcept>

namespace apex
{
class APEXTOOLS_EXPORT XPathProcessor
{
public:
    XPathProcessor(QString filename);
    virtual ~XPathProcessor();
    bool transform(QString xpath, QString newValue);
    void save(QString filename);

private:
    xmlDocPtr doc;

    void changeNode(xmlNodePtr node, QString newValue);
    xmlNodeSetPtr getNodes(xmlDocPtr doc, QString xpath);
    void registerNamespaces(xmlXPathContextPtr xpathContext, QString prefix,
                            QString href);
};

struct XPathException : public std::invalid_argument {
    XPathException(QString title, QString message)
        : std::invalid_argument(QString(title + ": " + message).toStdString()),
          title(title),
          message(message)
    {
    }
    virtual ~XPathException() throw()
    {
    }
    const QString title;
    const QString message;
};
}

#endif // _EXPORL_SRC_LIB_APEXTOOLS_XML_XPATHPROCESSOR_H_
