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

#include "apextools/apexpaths.h"

#include "apextools/version.h"

#include "apextools/xml/xmltools.h"
#include "apextools/xml/xpathprocessor.h"

#include "common/global.h"

#include "interactiveparameters.h"

#include <QFileInfo>
#include <QTemporaryFile>

namespace apex
{

InteractiveParameters::InteractiveParameters(const QDomDocument &document) :
    document_(document)
{
    QDomElement base = document_.elementsByTagName(QSL("interactive")).item(0).toElement();

    for (QDomElement currentNode = base.firstChildElement(); !currentNode.isNull();
            currentNode = currentNode.nextSiblingElement()) {
        const QString tag = currentNode.tagName();
        if (tag != QL1S("entry"))
            qFatal("invalid tag: %s", qPrintable(tag));

        const QString xpath = currentNode.attribute(QSL("expression"));
        const QString type = currentNode.attribute(QSL("type"));
        const QString description = currentNode.attribute(QSL("description"));
        const QString defaultValue = currentNode.attribute(QSL("default"));

        if (xpath.isEmpty() && description.isEmpty())
            qCCritical(APEX_RS, "%s", qPrintable(QSL("%1: %2").arg(QObject::tr("Interactive"),
                    QObject::tr("Interactive entries should contain an expression or "
                        "a description"))));

        ValueType valueType;
        if (type == QL1S("int"))
            valueType = IntValue;
        else if (type == QL1S("double"))
            valueType = DoubleValue;
        else if (type == QL1S("bool"))
            valueType = BoolValue;
        else if (type == QL1S("string"))
            valueType = StringValue;
        else if (type == QL1S("combo"))
            valueType = ComboValue;
        else
            qFatal("invalid type: %s", qPrintable(type));

        mEntries.append(Entry(xpath, valueType, description, defaultValue));
    }
}

QString InteractiveParameters::tempFileName() const
{
    QTemporaryFile tmpFile;
    tmpFile.open();
    return tmpFile.fileName();
}

XPathProcessor* InteractiveParameters::getXPathProcessor(QString *filename)
{
    *filename = tempFileName();
    XmlUtils::writeDocument(document_, *filename);
    return new XPathProcessor(*filename);
}

void InteractiveParameters::finishXPathProcessor(XPathProcessor* xpathProcessor, const QString &filename)
{
    xpathProcessor->save(filename);
    apex::ApexPaths paths;
    QString fileschema = paths.GetExperimentSchemaPath();
    document_ = XmlUtils::parseDocument(filename, QUrl::fromLocalFile(fileschema));
    delete xpathProcessor;
    QFile(filename).remove();
}

void InteractiveParameters::applyExpressions(const QMap<QString, QString> &expressions)
{
    QString filename;
    XPathProcessor* xpathProcessor = getXPathProcessor(&filename);

    QList<QString> keys = expressions.keys();
    for (unsigned i = 0; i < unsigned(keys.count()); ++i) {
        const QString xpath = keys.at(i);

        for(int i = 0; i<mEntries.count(); ++i){
            Entry *tmpEntry = &mEntries[i];
            if(tmpEntry->xpath == xpath) {
                mEntries.removeAt(i);
                break;
            }
        }

        xpathProcessor->transform(xpath, expressions.value(xpath));
    }

    finishXPathProcessor(xpathProcessor, filename);
}

void InteractiveParameters::apply(const QStringList &entryResults, Callback* callback)
{
    QString filename;
    XPathProcessor* xpathProcessor = getXPathProcessor(&filename);

    for (unsigned i = 0; i < unsigned(mEntries.size()); ++i) {
        Entry *entry = &mEntries[i];
        const QString xpath = entry->xpath;
        entry->result = entryResults.at(i);
        entry->succeeded = false;

        if (xpath.isEmpty()) {
            qCDebug(APEX_RS, "Empty xpath expression found, not transforming");
            continue;
        }

        try {
            entry->succeeded = xpathProcessor->transform(xpath, entry->result);
        } catch (const std::exception &e) {
            callback->warning(QString::fromLocal8Bit(e.what()));
        }
    }

    finishXPathProcessor(xpathProcessor, filename);
}

QDomDocument InteractiveParameters::document() const
{
    return document_;
}

QList<InteractiveParameters::Entry> InteractiveParameters::entries() const
{
    return mEntries;
}

data::ParameterDialogResults* InteractiveParameters::results() const
{
    data::ParameterDialogResults* result = new data::ParameterDialogResults;

    Q_FOREACH (const Entry &entry, mEntries)
        result->push_back(data::ParameterDialogResult(entry.xpath,
                    entry.description, entry.result, entry.succeeded));

    return result;
}

} // namespace apex
