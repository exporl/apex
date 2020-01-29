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

#include "../exceptions.h"

#include "common/global.h"

#include "xmltools.h"

#include <QAbstractMessageHandler>
#include <QDateTime>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QQueue>
#include <QSaveFile>
#include <QTextDocument>
#include <QXmlSchema>
#include <QXmlSchemaValidator>

namespace apex
{

class MessageHandler : public QAbstractMessageHandler
{
protected:
    virtual void
    handleMessage(QtMsgType type, const QString &description,
                  const QUrl &identifier,
                  const QSourceLocation &sourceLocation) Q_DECL_OVERRIDE;
};

// MessageHandler ==============================================================

Q_LOGGING_CATEGORY(APEX_XML, "apex.xml")

void MessageHandler::handleMessage(QtMsgType type, const QString &description,
                                   const QUrl &identifier,
                                   const QSourceLocation &sourceLocation)
{
    Q_UNUSED(identifier);
    QTextDocument doc;
    doc.setHtml(description);
    switch (type) {
    case QtWarningMsg:
        qCWarning(APEX_XML, "%s:%lld: %s",
                  qPrintable(sourceLocation.uri().toString()),
                  sourceLocation.line(), qPrintable(doc.toPlainText()));
        break;
    case QtFatalMsg:
        qCCritical(APEX_XML, "%s:%lld: %s",
                   qPrintable(sourceLocation.uri().toString()),
                   sourceLocation.line(), qPrintable(doc.toPlainText()));
        break;
    case QtCriticalMsg:
    case QtDebugMsg:
    case QtInfoMsg:
        // ignored, shouldn't be generated anyway
        break;
    }
}

// XmlUtils ====================================================================

bool XmlUtils::writeDocument(const QDomDocument &doc, const QString &fileName)
{
    QSaveFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QFile::Truncate)) {
        return false;
    }
    file.write(doc.toByteArray(2));
    file.commit();
    return true;
}

QString XmlUtils::nodeToString(const QDomNode &node)
{
    QDomDocument doc;
    doc.appendChild(doc.importNode(node, true));
    return doc.toString();
}

QString XmlUtils::richText(const QDomElement &node)
{
    if (node.firstChild().isCDATASection()) {
        return node.firstChild().toCDATASection().data();
    }

    QDomDocument doc;
    QDomElement root = doc.createElement(QSL("dummy"));
    doc.appendChild(root);
    QDomNode imported = doc.importNode(node, true);
    while (imported.hasChildNodes())
        root.appendChild(imported.firstChild());
    QString result = doc.toString(0).trimmed();
    return result.mid(7, result.length() - 7 - 8).trimmed();
}

void XmlUtils::removeComments(QDomDocument *doc)
{
    QQueue<QDomNode> queue;
    queue.enqueue(doc->documentElement());
    while (!queue.isEmpty()) {
        QDomNode node = queue.dequeue();
        for (int i = node.childNodes().count(); i >= 0; --i) {
            QDomNode child = node.childNodes().at(i);
            if (child.hasChildNodes()) {
                queue.enqueue(child);
            } else if (child.nodeType() == QDomNode::CommentNode) {
                node.removeChild(child);
            }
        }
    }
}

QDomDocument XmlUtils::parseString(const QString &s, const QUrl &schemaUrl,
                                   const QString &fileName,
                                   bool doRemoveComments)
{
    if (schemaUrl.isValid()) {
        MessageHandler handler;
        QNetworkAccessManager accessManager;
        accessManager.setNetworkAccessible(
            QNetworkAccessManager::NotAccessible);
        QXmlSchema schema;
        schema.setMessageHandler(&handler);
        schema.setNetworkAccessManager(&accessManager);
        if (!schema.load(schemaUrl))
            throw ApexStringException(
                tr("Unable to load schema: %1").arg(schemaUrl.toString()));
        QXmlSchemaValidator validator(schema);
        validator.setMessageHandler(&handler);
        validator.setNetworkAccessManager(&accessManager);
        if (!validator.validate(s.toUtf8(), QUrl::fromLocalFile(fileName)))
            throw ApexStringException(tr("Unable to validate XML document: %1")
                                          .arg(schemaUrl.toString()));
    }

    QDomDocument doc;
    QString errorMsg;
    int errorLine = 0;
    int errorColumn = 0;
    if (!doc.setContent(s, false, &errorMsg, &errorLine, &errorColumn))
        throw ApexStringException(tr("Parsing XML document failed at %1:%2: %3")
                                      .arg(errorLine)
                                      .arg(errorColumn)
                                      .arg(errorMsg));

    if (doRemoveComments)
        removeComments(&doc);

    return doc;
}

QDomDocument XmlUtils::parseDocument(const QString &fileName,
                                     const QUrl &schemaUrl)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        throw ApexStringException(tr("Cannot open file %1").arg(fileName));
    return parseString(QString::fromUtf8(file.readAll()), schemaUrl, fileName,
                       false);
}

QDomElement XmlUtils::createTextElement(QDomDocument *doc, const QString &tag,
                                        const QString &value)
{
    QDomElement result = doc->createElement(tag);
    result.appendChild(doc->createTextNode(value));
    return result;
}

QDomElement XmlUtils::createTextElement(QDomDocument *doc, const QString &tag,
                                        double value)
{
    return createTextElement(doc, tag, QString::number(value));
}

QDomElement XmlUtils::createTextElement(QDomDocument *doc, const QString &tag,
                                        const QDateTime &value)
{
    return createTextElement(doc, tag, value.toString(Qt::ISODate));
}

QDomElement XmlUtils::createRichTextElement(QDomDocument *doc,
                                            const QString &tag,
                                            const QString &value)
{
    QString s = QSL("<") + tag + QSL(">") + value + QSL("</") + tag + QSL(">");
    return doc->importNode(parseString(s).documentElement(), true).toElement();
}

void XmlUtils::setText(QDomElement *element, const QString &value)
{
    for (int i = element->childNodes().count(); i >= 0; --i)
        element->removeChild(element->childNodes().at(i));
    element->appendChild(element->ownerDocument().createTextNode(value));
}
}
