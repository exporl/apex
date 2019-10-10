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

#ifndef _EXPORL_SRC_LIB_APEXTOOLS_XML_XMLTOOLS_H_
#define _EXPORL_SRC_LIB_APEXTOOLS_XML_XMLTOOLS_H_

#include "../global.h"

#include <QCoreApplication>
#include <QDomElement>
#include <QUrl>

namespace apex
{

Q_DECLARE_LOGGING_CATEGORY(APEX_XML)

class APEXTOOLS_EXPORT XmlUtils
{
    Q_DECLARE_TR_FUNCTIONS(XmlUtils)
public:
    static bool writeDocument(const QDomDocument &doc, const QString &fileName);
    static QString nodeToString(const QDomNode &node);
    static void removeComments(QDomDocument *doc);
    static QDomDocument parseString(const QString &toParse,
                                    const QUrl &schemaUrl = QUrl(),
                                    const QString &fileName = QString(),
                                    bool removeComments = true);
    static QDomDocument parseDocument(const QString &fileName,
                                      const QUrl &schemaUrl = QUrl());
    static QString richText(const QDomElement &node);

    static QDomElement createTextElement(QDomDocument *doc, const QString &tag,
                                         const QString &value);
    static QDomElement createTextElement(QDomDocument *doc, const QString &tag,
                                         double value);
    static QDomElement createTextElement(QDomDocument *doc, const QString &tag,
                                         const QDateTime &value);

    static QDomElement createRichTextElement(QDomDocument *doc,
                                             const QString &tag,
                                             const QString &value);

    static void setText(QDomElement *element, const QString &value);
};
}

#endif
