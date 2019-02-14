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

#include "rtresultsink.h"

#include "apexdata/experimentdata.h"
#include "apextools/apextools.h"
#include "apextools/xml/xmltools.h"
#include "runner/experimentrundelegate.h"

#include "../accessmanager.h"
#include "../apexcontrol.h"
#include "../gui/mainwindow.h"
#include "resultapi.h"

#include "commongui/webview.h"
#include "common/paths.h"
#include "common/websocketserver.h"

#include <QDomElement>
#include <QFileDialog>
#include <QMenuBar>
#include <QPrintDialog>
#include <QPrinter>
#include <QTimer>
#include <QUrl>
#include <QXmlStreamReader>

using namespace cmn;

namespace apex
{

class RTResultSinkPrivate
{
public:
    RTResultSinkPrivate()
    {
        webView.reset(new WebView());
        QDir rvDataDir = QDir(Paths::searchDirectory(QSL("resultsviewer"),
                                                     Paths::dataDirectories()));
        QStringList entries =
            rvDataDir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
        Q_FOREACH (const QString &entry, entries) {
            ApexTools::recursiveCopy(rvDataDir.filePath(entry),
                                     temporaryDirectory.path());
        }

        ApexTools::recursiveCopy(
            Paths::searchFile(QSL("js/polyfill.js"), Paths::dataDirectories()),
            QDir(temporaryDirectory.path()).filePath(QSL("js/")));
        ApexTools::recursiveCopy(
            Paths::searchFile(QSL("js/commonwebsocket.js"),
                              Paths::dataDirectories()),
            QDir(temporaryDirectory.path()).filePath(QSL("js/")));
    }

    QScopedPointer<WebView> webView;
    QScopedPointer<cmn::WebSocketServer> webSocketServer;
    QScopedPointer<ResultApi> resultApi;
    QScopedPointer<QPrinter> printer;

    QUrl page;
    QMap<QString, QString> resultParameters;
    QString extraScript;
    QTemporaryDir temporaryDirectory;
};

RTResultSink::RTResultSink(QUrl p_page, QMap<QString, QString> resultParameters,
                           QString extraScript)
    : d(new RTResultSinkPrivate)
{
    d->page = p_page;
    d->resultParameters = resultParameters;
    d->extraScript = extraScript;

    AccessManager am;
    ApexTools::recursiveCopy(am.prepare(d->page).toLocalFile(),
                             d->temporaryDirectory.path());
    d->page =
        QUrl(QDir(d->temporaryDirectory.path()).filePath(d->page.fileName()));
    d->page.setScheme(QSL("file"));

    connect(d->webView.data(), SIGNAL(hidden()), this, SIGNAL(viewClosed()));
    connect(d->webView.data(), SIGNAL(hidden()), this, SLOT(hide()));

    connect(d->webView.data(), SIGNAL(loadingFinished(bool)), this,
            SLOT(setup(bool)));
    d->webView->load(d->page);

    d->resultApi.reset(new ResultApi);
    connect(d->resultApi.data(), SIGNAL(exportToPdf()), this,
            SLOT(exportToPdf()));
    d->webSocketServer.reset(new WebSocketServer(QSL("RTResultSink")));
    d->webSocketServer->start();
    d->resultApi->registerBaseMethods(d->webSocketServer.data());
    d->webSocketServer->on(QSL("psignifit"), d->resultApi.data(),
                           QSL("psignifit(QString)"));
    d->webSocketServer->on(QSL("exportToPdf"), d->resultApi.data(),
                           QSL("exportToPdf()"));

    if (d->webView->menuBar()->actions().size() >= 1)
        d->webView->menuBar()->actions().at(0)->menu()->addAction(
            QSL("Print to pdf"), this, SLOT(exportToPdf()));
}

RTResultSink::~RTResultSink()
{
    delete d;
}

void RTResultSink::setup(bool ok)
{
    runJavaScript(QL1S("var api = new ResultApi('ws://127.0.0.1:") +
                  QString::number(d->webSocketServer->serverPort()) +
                  QL1S("', '") + d->webSocketServer->csrfToken() + QL1S("');"));
    runJavaScript("setTimeout(function() { if (typeof initialize === "
                  "'function') initialize(); }, 1500);");
    setJavascriptParameters(d->resultParameters);
    runJavaScript(d->extraScript);

    Q_EMIT loadingFinished(ok);
}

void RTResultSink::show()
{
    d->webView->show();
    ApexControl::Get().mainWindow()->quickWidgetBugHide();
}

void RTResultSink::hide()
{
    ApexControl::Get().mainWindow()->quickWidgetBugShow();
    d->webView->hide();
}

void RTResultSink::newStimulusParameters(const QVariantMap &parameters)
{
    if (parameters.isEmpty())
        return;

    QString pstring = "params = {\"";
    QMapIterator<QString, QVariant> it(parameters);
    while (it.hasNext()) {
        it.next();
        // jsParams.insert(it.key(), it.value().toString());
        bool isNumber;
        QString valuestr;
        (it.value()).toFloat(&isNumber); // if toFloat() fails because
                                         // it.value() is not a number, isNumber
                                         // is set to false

        if (!isNumber) {
            valuestr = "\"" + it.value().toString() + "\"";
        } else {
            valuestr = it.value().toString();
        }

        pstring += it.key() + "\": " + valuestr;

        if (it.hasNext()) {
            pstring.append(", \"");
        } else {
            pstring.append("};");
            pstring.append("prepare();");
        }
    }
    runJavaScript(pstring);
}

void RTResultSink::trialStarted()
{
    runJavaScript("trialStarted();");
}

void RTResultSink::stimulusStarted()
{
    runJavaScript("stimulusStarted();");
}

void RTResultSink::setJavascriptParameters(
    QMap<QString, QString> resultParameters) const
{
    if (!resultParameters.isEmpty()) {
        QString rpstring = "params = {\"";
        QMapIterator<QString, QString> it(resultParameters);
        while (it.hasNext()) {
            it.next();

            bool isNumber;
            QString valuestr;
            (it.value()).toFloat(&isNumber); // if toFloat() fails because
                                             // it.value() is not a number,
                                             // isNumber is set to false

            if (!isNumber) {
                valuestr = "\"" + it.value() + "\"";
            } else {
                valuestr = it.value();
            }

            rpstring += it.key() + "\": " + valuestr;

            if (it.hasNext()) {
                rpstring.append(", \"");
            } else {
                rpstring.append("};");
            }
        }
        runJavaScript(rpstring);
    }
}

void RTResultSink::newAnswer(QString xml, bool doPlot)
{
    runJavaScript("newAnswer('" + ApexTools::escapeJavascriptString(xml) +
                  "');");
    if (doPlot)
        plot();
}

void RTResultSink::plot()
{
    runJavaScript("plot();");
}

void RTResultSink::newResults(QString xml)
{
    QDomElement document = XmlUtils::parseString(xml).documentElement();

    for (QDomElement currentNode = document.firstChildElement("trial");
         !currentNode.isNull();
         currentNode = currentNode.nextSiblingElement("trial")) {

        QString value;
        QTextStream stream(&value);
        currentNode.save(stream, 4);

        newAnswer(value.trimmed(), false);
    }

    plot();
}

QString RTResultSink::runJavaScript(QString script) const
{
    return d->webView->runJavaScript(script).toString();
}

void RTResultSink::exportToPdf()
{
#if !defined(Q_OS_ANDROID)
    QString filename(QFileDialog::getSaveFileName(
        0, tr("Export results page to PDF"), "", "*.pdf"));

    d->printer.reset(new QPrinter);
    d->printer->setOutputFormat(QPrinter::PdfFormat);
    d->printer->setOutputFileName(filename);
#if defined(WITH_WEBENGINE)
    d->webView->webView()->page()->print(d->printer.data(),
                                         [&](bool) { d->printer.reset(); });
#else
    d->webView->webView()->print(d->printer.data());
    d->printer.reset();
#endif
#endif
}

QByteArray RTResultSink::createCSVtext(const QString &resultFilePath)
{
    // This function should run the javascript script and put
    // the output in a variable so it can be added to a file later.
    // Load the file with the results
    // Open the xml file and load the contents

    QFile resultsfile(resultFilePath);
    resultsfile.open(QIODevice::ReadOnly);
    if (!resultsfile.isOpen()) {
        qCWarning(APEX_RS, "resultsfile could not be loaded");
        return QByteArray();
    }
    QString xmlString = resultsfile.readAll();

    QTemporaryDir temporaryDirectory;

    ApexTools::recursiveCopy(
        Paths::searchDirectory(QSL("resultsviewer"), Paths::dataDirectories()),
        temporaryDirectory.path());

    WebView localWebView;
    QUrl url =
        QDir(temporaryDirectory.path()).filePath("resultsviewer/apr2csv.html");
    url.setScheme(QSL("file"));
    localWebView.load(url);

    QEventLoop loop;
    QTimer timer;
    connect(&localWebView, SIGNAL(loadingFinished(bool)), &loop, SLOT(quit()));
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    timer.start(1000);
    loop.exec();

    return localWebView
        .runJavaScript("process(\"" +
                       ApexTools::escapeJavascriptString(xmlString) + "\")")
        .toByteArray();
}
}
