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
#include "apexdata/experimentdata.h"
#include "flowapi.h"
#include "accessmanager.h"

#include <QDebug>
#include <QWebFrame>
#include <QTemporaryFile>
#include <QPointer>
#include <QFileInfo>

using namespace apex;

bool FlowRunner::select(const QString &path)
{
    Q_EMIT opened(path);

    QTemporaryFile tmpFile;
    QTextStream tmpStream(&tmpFile);
    tmpFile.setFileTemplate("tmp-XXXXXX.html");
    tmpFile.open();

    QFile originalFile(path);
    originalFile.open(QIODevice::ReadOnly);
    tmpStream << originalFile.readAll();

    api =  new FlowApi(this, QFileInfo(path).absoluteDir());

    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalStorageEnabled, true);

    view = new QWebView();

    view->resize(480, 640);
    view->setFocus();
    view->page()->mainFrame()->addToJavaScriptWindowObject("api", api);
    QPointer<AccessManager> am = new AccessManager(view->page());
    view->page()->setNetworkAccessManager(am);
    view->page()->mainFrame()->load(QUrl::fromLocalFile(tmpFile.fileName()));

    view->showMaximized();

    return true;
}

void FlowRunner::selectFromDir(const QString &path)
{
    qCDebug(APEX_RS) << "Running SelectFromDir with " << path;
}

void FlowRunner::select(data::ExperimentData *data)
{
    makeInvisible();
    Q_EMIT selected(data);
}

void FlowRunner::makeInvisible()
{
    view->setVisible(false);
}

void FlowRunner::makeVisible()
{
    view->setVisible(true);
    view->setFocus();
}
