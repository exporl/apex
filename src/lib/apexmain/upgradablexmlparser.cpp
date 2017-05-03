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

#include "apextools/version.h"

#include "apextools/xml/xmltools.h"

#include "common/exception.h"

#include "upgradablexmlparser.h"

#include <QMetaMethod>
#include <QRegExp>

using namespace cmn;

namespace apex
{

UpgradableXmlParser::UpgradableXmlParser(const QString &fileName,
        const QString &schema,
        const QString &schemaUrl,
        const QString &nameSpace) :
    fileName(fileName),
    schema(schema),
    schemaUrl(schemaUrl),
    nameSpace(nameSpace)
{
}

UpgradableXmlParser::~UpgradableXmlParser()
{
}

QDomDocument UpgradableXmlParser::loadAndUpgradeDom(bool removeComments)
{
    // the code cannot cope with version parts that are not single digits
    Q_ASSERT(strlen(APEX_SCHEMA_VERSION) == 5);

    document = XmlUtils::parseDocument(fileName);

    QRegExp re("http://med.kuleuven.be/exporl/apex/(\\d+\\.\\d+\\.\\d+)/");
    if (re.lastIndexIn(document.documentElement().attribute("xmlns:apex")) == -1)
        throw Exception(tr("Cannot parse XML namespace of experiment %1")
                .arg(fileName));

    QString version = re.capturedTexts()[1];
    if (version == "3.0")
        version = "3.0.0";

    const QMap<QByteArray, QByteArray> methods = upgradeMethods();
    for (auto i = methods.lowerBound(version.toUtf8());
            i != methods.end(); ++i) {
        if (!metaObject()->invokeMethod(this, i.value()))
            throw Exception(tr("Unable to invoke upgrade method"));
    }
    document.documentElement().setAttribute("xmlns:apex", nameSpace);
    document.documentElement().setAttribute("xsi:schemaLocation",
            nameSpace + QL1S(" ") + schemaUrl);

    if (version != QL1S(APEX_SCHEMA_VERSION))
        qCWarning(APEX_RS, "%s", qPrintable(QSL("%1: %2").arg(tr("XML parser"),
                tr("XML file upgraded from previous version"))));

    document = XmlUtils::parseString(
        document.toString(), QUrl::fromLocalFile(schema), fileName, removeComments);

    return document;
}

QMap<QByteArray, QByteArray> UpgradableXmlParser::upgradeMethods() const
{
    QMap<QByteArray, QByteArray> result;

    for (int i = metaObject()->methodOffset();
         i < metaObject()->methodCount(); ++i) {
        const QMetaMethod metaMethod = metaObject()->method(i);
        if (metaMethod.access() != QMetaMethod::Private ||
            !metaMethod.parameterTypes().isEmpty() ||
            qstrcmp(metaMethod.typeName(), "void") ||
            metaMethod.methodType() != QMetaMethod::Slot)
            continue;

        QByteArray signature = metaMethod.methodSignature();
        if (!signature.startsWith("upgrade"))
            continue;
        Q_ASSERT(signature.size() == 7 + 5 + 2);
        signature.chop(2);
        result.insert(signature.mid(7).replace("_", "."), signature);
    }

    return result;
}

}
