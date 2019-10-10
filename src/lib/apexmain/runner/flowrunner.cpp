/******************************************************************************
 * Copyright (C) 2016 Jonas Vanthornhout <jonasvanthornhout+apex@gmail.com>   *
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

#include "flowrunner.h"

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
    QScopedPointer<WebView> webView;
    QScopedPointer<WebSocketServer> webSocketServer;
    QTemporaryDir temporaryDirectory;
};

FlowRunner::FlowRunner() : d(new FlowRunnerPrivate)
{
    ApexTools::recursiveCopy(
        Paths::searchDirectory(QSL("resultsviewer/external"),
                               Paths::dataDirectories()),
        d->temporaryDirectory.path());
    QFile::copy(
        ":/qtwebchannel/qwebchannel.js",
        QDir(d->temporaryDirectory.path()).filePath(QSL("qwebchannel.js")));
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

    d->flowApi.reset(new FlowApi(this, QFileInfo(path).absoluteDir()));

    connect(this, &FlowRunner::experimentDone, d->flowApi.data(),
            &FlowApi::experimentDone);

    try {
        d->webSocketServer.reset(
            new WebSocketServer(d->flowApi.data(), QSL("flowApi")));
    } catch (std::exception &e) {
        qCWarning(APEX_RS, "Unable to setup flowrunner %s.", e.what());
        return false;
    }

    d->webView.reset(new WebView);
    connect(d->webView.data(), SIGNAL(hidden()), this, SLOT(cleanup()));

    QString htmlPath =
        ApexTools::copyAndPrepareAsHtmlFileWithInjectedBootstrapValues(
            path, d->temporaryDirectory.path(),
            d->webSocketServer->serverPort());
    d->webView->load(QUrl::fromLocalFile(htmlPath));
    makeVisible();

    return true;
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

void FlowRunner::select(data::ExperimentData *data)
{
    Q_EMIT selected(data);
    makeInvisible();
}

void FlowRunner::makeInvisible()
{
    ApexControl::Get().mainWindow()->quickWidgetBugShow();
    d->webView->hide();
}

void FlowRunner::makeVisible()
{
    d->webView->showMaximized();
    ApexControl::Get().mainWindow()->quickWidgetBugHide();
}
} // namespace apex
