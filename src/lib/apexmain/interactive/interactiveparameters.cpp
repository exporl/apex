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

#include "apextools/services/paths.h"

#include "apextools/version.h"

#include "apextools/xml/apexxmltools.h"
#include "apextools/xml/xercesinclude.h"
#include "apextools/xml/xmldocumentgetter.h"
#include "apextools/xml/xpathprocessor.h"

#include "services/errorhandler.h"

#include "interactiveparameters.h"

#include <QFileInfo>
#include <QTemporaryFile>

using namespace xercesc;
using namespace apex::ApexXMLTools;

namespace apex
{

InteractiveParameters::InteractiveParameters(DOMDocument* document, XMLDocumentGetter& documentGetter) :
    document_(document),
    documentGetter(documentGetter)
{
    processXml();
}

void InteractiveParameters::processXml()
{
    const DOMElement * const base =
        XMLutils::GetElementsByTagName(document_->getDocumentElement(),
                QLatin1String("interactive"));

    if (!base)
        return;

    for (DOMNode *currentNode = base->getFirstChild(); currentNode != NULL;
            currentNode = currentNode->getNextSibling()) {
        Q_ASSERT(currentNode->getNodeType() == DOMNode::ELEMENT_NODE);

        const QString tag = XMLutils::GetTagName(currentNode);
        if (tag != QLatin1String("entry"))
            qFatal("invalid tag: %s", qPrintable(tag));

        const QString xpath = XMLutils::GetAttribute(currentNode,
                QLatin1String("expression"));
        const QString type = XMLutils::GetAttribute(currentNode,
                QLatin1String("type"));
        const QString description = XMLutils::GetAttribute(currentNode,
                QLatin1String("description"));
        const QString defaultValue = XMLutils::GetAttribute(currentNode,
                QLatin1String("default"));

        if (xpath.isEmpty() && description.isEmpty())
            ErrorHandler::Get().addError(QObject::tr("Interactive"),
                    QObject::tr("Interactive entries should contain an expression or "
                        "a description"));

        ValueType valueType;
        if (type == QLatin1String("int"))
            valueType = IntValue;
        else if (type == QLatin1String("double"))
            valueType = DoubleValue;
        else if (type == QLatin1String("bool"))
            valueType = BoolValue;
        else if (type == QLatin1String("string"))
            valueType = StringValue;
        else if (type == QLatin1String("combo"))
            valueType = ComboValue;
        else
            qFatal("invalid type: %s", qPrintable(type));

        mEntries.append(Entry(xpath, valueType, description,
                    defaultValue));
    }
}

QString InteractiveParameters::createTmpFile() const {
    // Apparently Windows can't handle files that are opened by two processes, so we create a seperate scope for the
    //  file, store the name and delete it ourselves
    QString tmpFilename;
    {
        QTemporaryFile tmpFile;
        tmpFile.open();
        QFileInfo tmpFileInfo(tmpFile);

        tmpFilename = tmpFileInfo.absoluteFilePath();
    }

    return tmpFilename;
}

void InteractiveParameters::deleteFile(QString tmpFilename) const {
    // Deletion of the temporary file
    QFile tmpFile(tmpFilename);
    tmpFile.remove();
}

XPathProcessor* InteractiveParameters::getXPathProcessor(QString& filename) {
    filename = createTmpFile();
    XMLutils::WriteElement(document_->getDocumentElement(), filename);
    return new XPathProcessor(filename);
}

void InteractiveParameters::finishXPathProcessor(XPathProcessor* xpathProcessor, QString filename) {
    xpathProcessor->save(filename);
    apex::Paths paths;
    QString fileschema = paths.GetExperimentSchemaPath();
    document_ = documentGetter.GetXMLDocument(filename, fileschema, EXPERIMENT_NAMESPACE);

    delete xpathProcessor;
    deleteFile(filename);
}

void InteractiveParameters::applyExpressions(QMap<QString, QString> expressions)
{
    QString filename;
    XPathProcessor* xpathProcessor = getXPathProcessor(filename);

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

void InteractiveParameters::apply(QStringList entryResults, Callback* callback)
{
    QString filename;
    XPathProcessor* xpathProcessor = getXPathProcessor(filename);

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
        } catch(const XPathException& e) {
            callback->warning(e.title, e.message);
        }
    }

    finishXPathProcessor(xpathProcessor, filename);
}

DOMDocument* InteractiveParameters::document() const {
    return document_;
}

const QList<InteractiveParameters::Entry>& InteractiveParameters::entries() const {
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
