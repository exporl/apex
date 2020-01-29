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

#include "common/paths.h"
#include "common/websocketserver.h"

#include <QDomElement>
#include <QFileDialog>
#include <QMenuBar>
#include <QUrl>

using namespace cmn;

namespace apex
{

class RTResultSinkPrivate
{
public:
    RTResultSinkPrivate()
    {
        QDir dir = QDir(Paths::searchDirectory(QSL("resultsviewer"),
                                               Paths::dataDirectories()));

        QStringList entries =
            dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
        Q_FOREACH (const QString &entry, entries) {
            ApexTools::recursiveCopy(dir.filePath(entry),
                                     temporaryDirectory.path());
        }

        QFile::copy(
            ":/qtwebchannel/qwebchannel.js",
            QDir(temporaryDirectory.path()).filePath(QSL("qwebchannel.js")));
    }

    QScopedPointer<WebView> webView;
    QScopedPointer<WebSocketServer> webSocketServer;
    QScopedPointer<ResultApi> resultApi;

    QTemporaryDir temporaryDirectory;
};

RTResultSink::RTResultSink(QUrl url, QMap<QString, QString> resultParameters,
                           QString extraScript)
    : d(new RTResultSinkPrivate)
{
    d->resultApi.reset(new ResultApi);
    d->webSocketServer.reset(
        new WebSocketServer(d->resultApi.data(), QSL("resultApi")));

    d->webView.reset(new WebView());
    connect(d->webView.data(), SIGNAL(hidden()), this, SIGNAL(viewClosed()));
    connect(d->webView.data(), SIGNAL(hidden()), this, SLOT(hide()));
    connect(d->resultApi.data(), SIGNAL(doHide()), this, SLOT(hide()));
    connect(d->resultApi.data(), SIGNAL(doHide()), d->webView.data(),
            SIGNAL(hidden()));
    connect(d->webView.data(), &WebView::loadingFinished,
            [this, resultParameters, extraScript](bool ok) {
                setJavascriptParameters(resultParameters);
                runJavaScript(extraScript);
                emit loadingFinished(ok);
            });

    QString htmlPath =
        ApexTools::copyAndPrepareAsHtmlFileWithInjectedBootstrapValues(
            AccessManager::toLocalFile(
                url, cmn::Paths::searchDirectory(
                         QSL("resultsviewer"), cmn::Paths::dataDirectories())),
            d->temporaryDirectory.path(), d->webSocketServer->serverPort());
    d->webView->load(QUrl::fromLocalFile(htmlPath));

    if (d->webView->menuBar()->actions().size() >= 1)
        d->webView->menuBar()->actions().at(0)->menu()->addAction(
            QSL("Print"), this, &RTResultSink::print);
}

RTResultSink::~RTResultSink()
{
    delete d;
}

void RTResultSink::show()
{
    d->webView->showMaximized();
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

void RTResultSink::runJavaScript(const QString &script) const
{
    d->webView->runJavaScript(script);
}

WebView &RTResultSink::getWebView() const
{
    return *d->webView.data();
}

void RTResultSink::print()
{
    d->webView->getWebViewWidget()->print();
}
}
