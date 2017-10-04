/******************************************************************************
 * Copyright (C) 2016 Jonas Vanthornhout <jonasvanthornhout+apex@gmail.com>   *
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

#include "flowrunner.h"

#include "accessmanager.h"
#include "apexcontrol.h"
#include "flowapi.h"
#include "gui/mainwindow.h"

#include "apexdata/experimentdata.h"

#include "apextools/apextools.h"

#include "common/paths.h"

#include <QFileInfo>
#include <QTemporaryDir>

using namespace cmn;

namespace apex
{
class FlowRunnerPrivate
{
public:
    QScopedPointer<FlowApi> flowApi;
    QScopedPointer<cmn::WebView> webView;
    QScopedPointer<cmn::WebSocketServer> webSocketServer;
    QTemporaryDir temporaryDirectory;
};

FlowRunner::FlowRunner() : d(new FlowRunnerPrivate)
{
    ApexTools::recursiveCopy(
        Paths::searchFile(QSL("js/flowapi.js"), Paths::dataDirectories()),
        QDir(d->temporaryDirectory.path()).filePath(QSL("js/")));
    ApexTools::recursiveCopy(
        Paths::searchFile(QSL("js/commonwebsocket.js"),
                          Paths::dataDirectories()),
        QDir(d->temporaryDirectory.path()).filePath(QSL("js/")));
    ApexTools::recursiveCopy(
        Paths::searchFile(QSL("js/polyfill.js"), Paths::dataDirectories()),
        QDir(d->temporaryDirectory.path()).filePath(QSL("js/")));
    ApexTools::recursiveCopy(
        Paths::searchDirectory(QSL("resultsviewer/external"),
                               Paths::dataDirectories()),
        d->temporaryDirectory.path());
}

FlowRunner::~FlowRunner()
{
    delete d;
}

bool FlowRunner::select(const QString &path)
{
    Q_EMIT opened(path);

    if (!QFile::exists(path)) {
        qCWarning(APEX_RS, "%s could not be found", qPrintable(path));
        return false;
    }
    connect(this, SIGNAL(savedFile(QString)), this, SLOT(onSavedFile(QString)));

    d->flowApi.reset(new FlowApi(this, QFileInfo(path).absoluteDir()));
    d->webSocketServer.reset(new WebSocketServer(QSL("FlowRunner")));
    d->webSocketServer->start();
    d->webSocketServer->on(QSL("runExperiment"), d->flowApi.data(),
                           QSL("runExperiment(QString,QString)"));
    d->webSocketServer->on(QSL("addExpression"), d->flowApi.data(),
                           QSL("addExpression(QString,QString)"));
    d->webSocketServer->on(QSL("clearExpressions"), d->flowApi.data(),
                           QSL("clearExpressions()"));
    d->webSocketServer->on(QSL("absoluteFilePath"), d->flowApi.data(),
                           QSL("absoluteFilePath(QString)"));
    d->webSocketServer->on(QSL("readFile"), d->flowApi.data(),
                           QSL("readFile(QString)"));

    d->webView.reset(new WebView);
    connect(d->webView.data(), SIGNAL(loadingFinished()), this,
            SLOT(setupView()));
    connect(d->webView.data(), SIGNAL(hidden()), this, SLOT(cleanup()));

    QUrl url = QUrl::fromUserInput(
        QDir(d->temporaryDirectory.path())
            .filePath(QFileInfo(path).baseName() + QL1S(".html")));
    ApexTools::recursiveCopy(path, url.toString(QUrl::RemoveScheme));
    d->webView->load(url);

    return true;
}

void FlowRunner::setupView()
{
    d->webView->runJavaScript(
        QL1S("var api = new FlowApi('ws://127.0.0.1:") +
        QString::number(d->webSocketServer->serverPort()) + QL1S("', '") +
        d->webSocketServer->csrfToken() + QL1S("');"));
    d->webView->runJavaScript(
        "setTimeout(function() { initialize(); }, 1000);");
    makeVisible();
}

void FlowRunner::cleanup()
{
    d->webSocketServer.reset();
    ApexControl::Get().mainWindow()->quickWidgetBugShow();
}

void FlowRunner::selectFromDir(const QString &path)
{
    qCDebug(APEX_RS) << "Running SelectFromDir with " << path;
}

void FlowRunner::onSavedFile(const QString &filePath)
{
    if (d->webSocketServer.isNull())
        return;

    QVariantList arguments;
    arguments << QVariant(filePath);
    d->webSocketServer->broadcastMessage(
        WebSocketServer::buildInvokeMessage(QSL("savedFile"), arguments));
}

void FlowRunner::select(data::ExperimentData *data)
{
    makeInvisible();
    Q_EMIT selected(data);
}

void FlowRunner::makeInvisible()
{
    ApexControl::Get().mainWindow()->quickWidgetBugShow();
    d->webView->hide();
}

void FlowRunner::makeVisible()
{
    d->webView->show();
    ApexControl::Get().mainWindow()->quickWidgetBugHide();
}
}
