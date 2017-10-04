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

#include "rtresultsink.h"

#include "apexdata/experimentdata.h"
#include "apextools/apextools.h"
#include "runner/experimentrundelegate.h"

#include "../accessmanager.h"
#include "../apexcontrol.h"
#include "../gui/mainwindow.h"
#include "resultapi.h"

#include "common/websocketserver.h"

#include <QFileDialog>
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
    ApexTools::recursiveCopy(am.prepare(d->page).toString(QUrl::RemoveScheme),
                             d->temporaryDirectory.path());
    d->page =
        QUrl(QDir(d->temporaryDirectory.path()).filePath(d->page.fileName()));
    d->page.setScheme(QSL("file"));
    connect(d->webView.data(), SIGNAL(loadingFinished(bool)), this,
            SIGNAL(loadingFinished(bool)));
    connect(d->webView.data(), SIGNAL(loadingFinished(bool)), this,
            SLOT(setup()));
    connect(d->webView.data(), SIGNAL(hidden()), this, SIGNAL(viewClosed()));
    connect(d->webView.data(), SIGNAL(hidden()), this, SLOT(hide()));

    d->webView->load(d->page);
    d->resultApi.reset(new ResultApi);
    connect(d->resultApi.data(), SIGNAL(exportToPdf()), this,
            SLOT(exportToPdf()));
    d->webSocketServer.reset(new WebSocketServer(QSL("RTResultSink")));
    d->webSocketServer->start();
    d->webSocketServer->on(QSL("psignifit"), d->resultApi.data(),
                           QSL("psignifit(QString)"));
    d->webSocketServer->on(QSL("exportToPdf"), d->resultApi.data(),
                           QSL("exportToPdf()"));
}

RTResultSink::~RTResultSink()
{
    delete d;
}

void RTResultSink::setup()
{
    d->webView->runJavaScript(
        QL1S("var api = new ResultApi('ws://127.0.0.1:") +
        QString::number(d->webSocketServer->serverPort()) + QL1S("', '") +
        d->webSocketServer->csrfToken() + QL1S("');"));
    setJavascriptParameters(d->resultParameters);
    executeJavaScript(d->extraScript);
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
    executeJavaScript(pstring);
}

void RTResultSink::trialStarted()
{
    executeJavaScript("trialStarted();");
}

void RTResultSink::stimulusStarted()
{
    executeJavaScript("stimulusStarted();");
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
        executeJavaScript(rpstring);
    }
}

void RTResultSink::executeJavaScript(QString JScode) const
{
    d->webView->runJavaScript(JScode);
}

void RTResultSink::newAnswer(QString xml, bool doPlot)
{
    qCDebug(APEX_RS, "RTResultSink::newAnswer");
    QString code("newAnswer(\"" + ApexTools::escapeJavascriptString(xml) +
                 "\");");
    if (doPlot)
        code += "plot();";
    d->webView->runJavaScript(code);
}

void RTResultSink::plot()
{
    d->webView->runJavaScript("plot();");
}

void RTResultSink::newResults(QString xml)
{
    qCDebug(APEX_RS, "RTResultSink::newResults");

    QXmlStreamReader xsr(xml);
    if (xsr.error() != QXmlStreamReader::NoError)
        qCDebug(APEX_RS, "XMLStreamReader error: %s",
                qPrintable(xsr.errorString()));
    while (!xsr.atEnd()) {
        qint64 start = xsr.characterOffset();
        xsr.readNext();
        if (xsr.error() != QXmlStreamReader::NoError)
            qCDebug(APEX_RS, "XMLStreamReader error: %s",
                    qPrintable(xsr.errorString()));
        if (xsr.isStartElement() && xsr.name() == "trial") {
            while (!(xsr.isEndElement() && xsr.name() == "trial")) {
                xsr.readNext();
            }
            xsr.readNext();
            qint64 end = xsr.characterOffset();
            QString text(xml.mid(start - 1, end - start - 1));
            newAnswer(text, false);
        }
    }
}

QVariant RTResultSink::evaluateJavascript(QString script)
{
    QEventLoop loop;
    QTimer *timeoutTimer = new QTimer();
    QVariant jsResult;
    connect(d->webView.data(), &WebView::javascriptFinished,
            [&](const QVariant &result) { jsResult = result; });
    connect(timeoutTimer, SIGNAL(timeout()), &loop, SLOT(quit()));
    d->webView->runJavaScript(script);
    timeoutTimer->start(5000);
    loop.exec();

    return jsResult;
}

void RTResultSink::exportToPdf()
{
#ifndef Q_OS_ANDROID
    QString filename(QFileDialog::getSaveFileName(
        0, tr("Export results page to PDF"), "", "*.pdf"));

    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filename);
    d->webView->webView()->print(&printer);
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
    QTimer *timeoutTimer = new QTimer();
    connect(&localWebView, SIGNAL(loadingFinished(bool)), &loop, SLOT(quit()));
    connect(timeoutTimer, SIGNAL(timeout()), &loop, SLOT(quit()));
    timeoutTimer->start(1000);
    loop.exec();

    QVariant jsResult;
    connect(&localWebView, &WebView::javascriptFinished,
            [&](const QVariant &result) { jsResult = result; });
    connect(timeoutTimer, SIGNAL(timeout()), &loop, SLOT(quit()));
    localWebView.runJavaScript(
        "process(\"" + ApexTools::escapeJavascriptString(xmlString) + "\")");
    timeoutTimer->start(5000);
    loop.exec();

    return jsResult.toByteArray();
}
}
