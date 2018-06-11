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

#include "apextools/apexpaths.h"
#include "apextools/apextools.h"
#include "apextools/exceptions.h"

#include "apextools/xml/xmltools.h"

#include "common/exception.h"
#include "common/global.h"
#include "common/paths.h"

#include "mainconfigfileparser.h"

#include "scriptexpander.h"
#include "xmlpluginapi.h"

#include <QFileDialog>
#include <QMap>
#include <QMessageBox>
#include <QScriptEngine>
#include <QScriptEngineDebugger>
#include <QString>

using namespace cmn;

namespace apex
{
namespace parser
{

const QString ScriptExpander::INSTANCENAME("pluginINTERFACE");

ScriptExpander::ScriptExpander(const QString &fileName,
                               const QString &libraryFile,
                               const QVariantMap &scriptParameters,
                               QWidget *parent)
    : fileName(fileName),
      m_libraryFile(libraryFile),
      m_scriptParameters(scriptParameters),
      m_parent(parent)
{
}

void ScriptExpander::ExpandScript(const QDomNode &base, const QString &function)
{
    // parse XML into localscript and parameters. Localscript is the script
    // itself, either from inline or read from file
    QVariantMap parameters(m_scriptParameters);
    QString localscript;
    bool fromgeneral = true;

    for (QDomElement it = base.firstChildElement(); !it.isNull();
         it = it.nextSiblingElement()) {
        const QString tag(it.tagName());
        if (tag == "parameter") {
            QString name = it.attribute(QSL("name"));
            QVariant value = it.text();
            parameters[name] = value;
            fromgeneral = false;
        } else if (tag == "script") {
            QString source = it.attribute(QSL("source"));
            QString value = it.text();
            // get script (inline or from file)
            if (source == "inline") {
                localscript = value;
            } else if (source == "file") {
                QFile file(Paths::searchFile(
                    value, QStringList()
                               << QDir::currentPath()
                               << ApexPaths::GetNonBinaryPluginPath()));
                if (!file.open(QIODevice::ReadOnly))
                    throw Exception(
                        tr("Cannot open plugin script: %1").arg(value));
                localscript = file.readAll();
                fromgeneral = false;
            } else {
                throw Exception(tr("Invalid script source attribute value"));
            }
        }
    }

    if (fromgeneral && m_libraryFile.isEmpty())
        throw Exception(
            tr("No script library present. It should be defined in <general>"));

    // Get the script api and attach it to the current script
    if (!m_libraryFile.isEmpty()) {
        QFile file(Paths::searchFile(
            m_libraryFile, QStringList()
                               << QDir::currentPath()
                               << ApexPaths::GetNonBinaryPluginPath()));
        if (!file.open(QIODevice::ReadOnly))
            throw Exception(
                tr("Cannot open script library file: %1").arg(m_libraryFile));
        localscript = file.readAll() + "\n" + localscript;
    }

    // attach the main script library to the current script
    QString mainPluginLibrary(
        MainConfigFileParser::Get().data().pluginScriptLibrary());
    if (!mainPluginLibrary.isEmpty()) {
        QFile file(Paths::searchFile(
            mainPluginLibrary, QStringList()
                                   << ApexPaths::GetNonBinaryPluginPath()));
        if (!file.open(QIODevice::ReadOnly))
            throw Exception(tr("Cannot open main script library file: %1")
                                .arg(mainPluginLibrary));
        localscript = file.readAll() + "\n" + localscript;
    }

    qCDebug(APEX_RS, "Executing XML plugin script");

    QScriptEngine m_scriptEngine;
    QScriptEngineDebugger m_scriptEngineDebugger;
    m_scriptEngineDebugger.attachTo(&m_scriptEngine);

    // for consistency, add a console.log function that proxies to the internal
    // print method
    m_scriptEngine.evaluate("console = { log: function() { print.apply(0, "
                            "Array.prototype.slice.call(arguments, 0)); } }");

    // create API
    XMLPluginAPI api;
    QScriptValue APIObject = m_scriptEngine.newQObject(&api);
    m_scriptEngine.globalObject().setProperty("api", APIObject);

    // set parameters in params object
    QScriptValue paramsObject = m_scriptEngine.newObject();
    m_scriptEngine.globalObject().setProperty("params", paramsObject);

    for (QMap<QString, QVariant>::const_iterator it = parameters.begin();
         it != parameters.end(); ++it) {
        paramsObject.setProperty(it.key(), ApexTools::QVariant2ScriptValue(
                                               &m_scriptEngine, it.value()));
    }

    if (!m_scriptEngine.canEvaluate(localscript))
        throw Exception("Can not evaluate script");

    // Evaluate script as is: load all functions etc.
    QScriptValue result = m_scriptEngine.evaluate(localscript);

    // check for errors from script execution
    if (m_scriptEngine.hasUncaughtException())
        throw Exception(tr("Could not execute script at line %1:\n%2")
                            .arg(m_scriptEngine.uncaughtExceptionLineNumber())
                            .arg(result.toString()));

    // evaluate given function
    m_scriptEngine.globalObject().setProperty(INSTANCENAME, result);

    qCDebug(APEX_RS, "=========== Evaluating %s ==================",
            qPrintable(function));
    result = m_scriptEngine.evaluate(function + "()");
    qCDebug(APEX_RS, "========== end evaluation ==============");

    if (m_scriptEngine.hasUncaughtException())
        throw Exception(tr("Could not execute script at line %1:\n%2")
                            .arg(m_scriptEngine.uncaughtExceptionLineNumber())
                            .arg(result.toString()));

    // replace current node by expanded version
    QDomNode parent = base.parentNode();
    QDomDocument doc = base.ownerDocument();
    QDomNode newNode =
        doc.importNode(XmlUtils::parseString(
                           QSL("<dummy>") + result.toString() + QSL("</dummy>"))
                           .documentElement(),
                       true);

    // take dummy element and add all children
    while (newNode.hasChildNodes())
        parent.insertBefore(newNode.firstChild(), base);

    parent.removeChild(base);

    // revalidate the document
    try {
        XmlUtils::parseString(
            doc.toString(),
            QUrl::fromLocalFile(ApexPaths::GetExperimentSchemaPath()),
            fileName);
    } catch (const std::exception &e) {
        if (m_parent) {
            int answer = QMessageBox::critical(
                m_parent, tr("Error expanding plugin"),
                tr("There was an error expanding a plugin, "
                   "would you like to save the resulting "
                   "XML document?"),
                QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
            if (answer == QMessageBox::Yes) {
                XmlUtils::writeDocument(
                    doc, QFileDialog::getSaveFileName(
                             m_parent, tr("Save XML document"), "output.xml"));
            }
        } else {
            qCDebug(APEX_RS, "No parent defined, not interactive");
        }
        throw;
    }
}
}
}
