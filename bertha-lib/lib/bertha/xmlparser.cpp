/******************************************************************************
 * Copyright (C) 2008 Michael Hofmann <mh21@piware.de>                        *
 * Copyright (C) 2008 Division of Experimental Otorhinolaryngology K.U.Leuven *
 *                                                                            *
 * Original version written by Maarten Lambert.                               *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 3 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with this program; if not, write to the Free Software Foundation, Inc.,    *
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                *
 ******************************************************************************/

#include "common/exception.h"

#include "xmlparser.h"

#include <QAbstractMessageHandler>
#include <QDebug>
#include <QDomDocument>
#include <QFile>
#include <QScopedPointer>
#include <QUrl>
#include <QXmlSchema>
#include <QXmlSchemaValidator>

using namespace cmn;

namespace bertha
{

class MessageHandler : public QAbstractMessageHandler
{
public:
    void handleMessage(QtMsgType type, const QString &description,
                       const QUrl &identifier,
                       const QSourceLocation &sourceLocation);

    QString lastError;
    QString lastWarning;
    QString lastMessage;
};

class XmlParserPrivate
{
public:
    XmlParserPrivate();

    QXmlSchema schema;
    MessageHandler messageHandler;
};

XmlParser::XmlParser(const QString &schemaPath) : d(new XmlParserPrivate)
{
    if (!d->schema.load(QUrl::fromLocalFile(schemaPath)))
        throw Exception(
            tr("Invalid schema:\n%1").arg(d->messageHandler.lastError));
}

XmlParser::~XmlParser()
{
    delete d;
}

QDomDocument XmlParser::parse(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        throw Exception(tr("Unable to open file %1").arg(filePath));
    return parse(file.readAll());
}

QDomDocument XmlParser::parse(const QByteArray &data)
{
    QXmlSchemaValidator validator(d->schema);
    if (!validator.validate(data))
        throw Exception(tr("Document does not validate against schema"));
    QDomDocument result;
    QString errorMessage;
    if (!result.setContent(data, &errorMessage))
        throw Exception(tr("Document is not valid: %1").arg(errorMessage));
    return result;
}

QString XmlParser::attribute(const QDomElement &element,
                             const QString &attribute) const
{
    return element.attributes().namedItem(attribute).nodeValue();
}

XmlParserPrivate::XmlParserPrivate()
{
    schema.setMessageHandler(&messageHandler);
}

void MessageHandler::handleMessage(QtMsgType type, const QString &description,
                                   const QUrl &identifier,
                                   const QSourceLocation &sourceLocation)
{
    Q_UNUSED(identifier);

    QString msg = QString::fromLatin1("%1\nFile: %2\nLine: %3, Column: %4")
                      .arg(description)
                      .arg(sourceLocation.uri().toString())
                      .arg(sourceLocation.line())
                      .arg(sourceLocation.column());

    switch (type) {
    case QtDebugMsg:
        lastMessage = msg;
        break;
#if QT_VERSION >= 0x050500
    case QtInfoMsg:
        lastMessage = msg;
        break;
#endif
    case QtWarningMsg:
        lastWarning = msg;
        break;
    case QtCriticalMsg:
    case QtFatalMsg:
        lastError = msg;
        break;
    }
}

}; // namespace bertha
