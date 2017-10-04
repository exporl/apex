/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
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

#ifndef _EXPORL_COMMON_LIB_COMMON_XMLUTILS_H_
#define _EXPORL_COMMON_LIB_COMMON_XMLUTILS_H_

#include "exception.h"
#include "global.h"

#include <QBuffer>
#include <QCoreApplication>
#include <QString>
#include <QXmlDefaultHandler>
#include <QXmlStreamReader>

namespace cmn
{

/** Returns indentation for XML writers. Returns @a indentSize * @a
 * indentLevel space characters.
 *
 * @param indentLevel indentation level
 * @param indentSize indentation size
 * @return string with @a indentSize * @a indentLevel space characters
 */
COMMON_EXPORT QString xmlIndent(unsigned indentLevel, unsigned indentSize = 4);

/** Escapes text for direct output to an XML file. Replaces lower than,
 * greater than and ampersands.
 *
 * @param text input text
 * @return text with entities replaced
 */
COMMON_EXPORT QString xmlEscapedText(const QString &text);

/** Escapes text for use as an XML attribute. Additionally to the work done
 * by #xmlEscapedText(), it replaces all existing double quotes and encloses
 * the whole string in double quotes.
 *
 * @param text input text
 * @return text with entities replaced and enclosed in quotes
 */
COMMON_EXPORT QString xmlEscapedAttribute(const QString &text);

#define XML_CHECK_ATTRIBUTE(attribute)                                         \
    do {                                                                       \
        if (!xml->attributes().hasAttribute(QL1S(#attribute))) {               \
            xml->raiseError(                                                   \
                tr("Attribute '" #attribute "' of '%1' element is missing.")   \
                    .arg(xml->name().toString()));                             \
            return;                                                            \
        }                                                                      \
        attribute = xml->attributes().value(QL1S(#attribute)).toString();      \
        if (attribute.isEmpty()) {                                             \
            xml->raiseError(                                                   \
                tr("Attribute '" #attribute "' of '%1' element is empty.")     \
                    .arg(xml->name().toString()));                             \
            return;                                                            \
        }                                                                      \
    } while (0)

#define XML_CHECK_OPTIONAL_ATTRIBUTE(attribute, defaultValue)                  \
    do {                                                                       \
        attribute = QSL(defaultValue);                                         \
        if (xml->attributes().hasAttribute(QL1S(#attribute))) {                \
            attribute = xml->attributes().value(QL1S(#attribute)).toString();  \
        }                                                                      \
    } while (0)

#define XML_CHECK_ELEMENT_NONEMPTY(element)                                    \
    do {                                                                       \
        if (!element.isEmpty()) {                                              \
            xml->raiseError(                                                   \
                tr("Element '" #element "' provided multiple times."));        \
            return;                                                            \
        }                                                                      \
        element = xml->readElementText();                                      \
        if (element.isEmpty()) {                                               \
            xml->raiseError(tr("Element '" #element "' is empty."));           \
            return;                                                            \
        }                                                                      \
    } while (0)

#define XML_CHECK_ELEMENT(element)                                             \
    do {                                                                       \
        if (!element.isEmpty()) {                                              \
            xml->raiseError(                                                   \
                tr("Element '" #element "' provided multiple times."));        \
            return;                                                            \
        }                                                                      \
        element = xml->readElementText();                                      \
    } while (0)

#define XML_CHECK_NOERROR                                                      \
    do {                                                                       \
        if (xml->hasError()) {                                                 \
            return;                                                            \
        }                                                                      \
    } while (0)

template <typename T>
QByteArray dumpXml(T &&generator)
{
    QBuffer buffer;
    // No QFile::Text so we always create linux line endings
    buffer.open(QIODevice::WriteOnly);
    QXmlStreamWriter xml(&buffer);
    xml.setAutoFormatting(true);
    generator.write(&xml);
    return buffer.data();
}

template <typename T>
typename T::Result loadXml(const QByteArray &contents, T &&handler)
{
    QBuffer buffer(const_cast<QByteArray *>(&contents));
    buffer.open(QIODevice::ReadOnly | QIODevice::Text);

    QXmlStreamReader xml(&buffer);

    handler.parse(&xml);

    if (xml.hasError())
        throw Exception(QCoreApplication::translate(
                            "loadXml", "Unable to load XML data: %1")
                            .arg(xml.errorString()));

    return handler.result();
}

template <typename T>
typename T::Result loadXml(QIODevice *device, T &&handler)
{
    QXmlStreamReader xml(device);

    handler.parse(&xml);

    if (xml.hasError())
        throw Exception(QCoreApplication::translate(
                            "loadXml", "Unable to load XML data: %1")
                            .arg(xml.errorString()));

    return handler.result();
}

} // namespace cmn

#endif
