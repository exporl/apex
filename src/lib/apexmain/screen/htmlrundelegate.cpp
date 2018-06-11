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

#include "apexdata/screen/htmlelement.h"

#include "apextools/apextools.h"

#include "parameters/parametermanager.h"

#include "runner/experimentrundelegate.h"

#include "gui/guidefines.h"

#include "screen/screenrundelegate.h"

#include "accessmanager.h"

#include "htmlapi.h"
#include "htmlrundelegate.h"

#include "commongui/webview.h"
#include "common/paths.h"
#include "common/websocketserver.h"

#include <QTemporaryDir>
#include <QVariant>

using namespace cmn;

// TODO: error handling (functions missing in javascript etc.)
namespace apex
{
namespace rundelegates
{

class HtmlRunDelegatePrivate
{
public:
    QScopedPointer<WebView> webView;
    QScopedPointer<WebSocketServer> webSocketServer;
    QScopedPointer<HtmlAPI> api;

    QTemporaryDir temporaryDirectory;
    AccessManager *accessManager;
    bool hasFinishedLoading;
};

const ScreenElement *HtmlRunDelegate::getScreenElement() const
{
    return element;
}

void HtmlRunDelegate::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::EnabledChange)
        enable();
}

void HtmlRunDelegate::setEnabled(bool enable)
{
    this->enable();
    ScreenElementRunDelegate::setEnabled(enable);
}

void HtmlRunDelegate::enable()
{
    QString code;
    if (!d->hasFinishedLoading)
        code = QSL("setTimeout(function() { reset(); enabled(); }, 1000);");
    else
        code = QSL("reset(); enabled();");
    d->webView->runJavaScript(code);
}

QWidget *HtmlRunDelegate::getWidget()
{
    return d->webView->webView();
}

bool HtmlRunDelegate::hasText() const
{
    return true;
}

bool HtmlRunDelegate::hasInterestingText() const
{
    return true;
}

const QString HtmlRunDelegate::getText() const
{
#if defined(WITH_WEBENGINE)
    QEventLoop loop;
    QTimer timer;
    QVariant result;
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    connect(d->api.data(), &HtmlAPI::javascriptFinished,
            [&](const QVariant &value) {
                result = value;
                loop.quit();
            });
    d->webSocketServer->broadcastMessage(WebSocketServer::buildInvokeMessage(
        QSL("evaluateJavaScript"), QVariantList() << QL1S("getResult();")));
    timer.start(10000);
    loop.exec();
    return result.toString();
#else
    return d->webView->runJavaScript("getResult();").toString();
#endif
}

void HtmlRunDelegate::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);
}

void HtmlRunDelegate::connectSlots(gui::ScreenRunDelegate *d)
{
    connect(this, SIGNAL(answered(ScreenElementRunDelegate *)), d,
            SIGNAL(answered(ScreenElementRunDelegate *)));
}

void HtmlRunDelegate::sendAnsweredSignal()
{
    Q_EMIT answered(this);
}

HtmlRunDelegate::HtmlRunDelegate(ExperimentRunDelegate *p_rd, QWidget *parent,
                                 const HtmlElement *e)
    : QObject(parent),
      ScreenElementRunDelegate(p_rd, e),
      element(e),
      d(new HtmlRunDelegatePrivate)
{
    d->hasFinishedLoading = false;
    ApexTools::recursiveCopy(
        Paths::searchFile(QSL("js/htmlapi.js"), Paths::dataDirectories()),
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

    d->api.reset(new HtmlAPI());
    d->webView.reset(new WebView());
    d->webSocketServer.reset(new WebSocketServer(QSL("HtmlRunDelegate")));
    d->webSocketServer->start();
    d->api->registerBaseMethods(d->webSocketServer.data());
    d->webSocketServer->on(QSL("answered"), this, QSL("sendAnsweredSignal()"));
    d->webSocketServer->on(QSL("javascriptFinished"), d->api.data(),
                           QSL("javascriptFinished(QVariant)"));

    d->accessManager = new AccessManager(this);
    connect(d->webView.data(), SIGNAL(loadingFinished(bool)), this,
            SLOT(setup()));

    QUrl sourcePage = d->accessManager->prepare(element->page());
    QString targetPage =
        QDir(d->temporaryDirectory.path())
            .filePath(QFileInfo(sourcePage.toString()).baseName() +
                      QL1S(".html"));
    ApexTools::recursiveCopy(sourcePage.toLocalFile(), targetPage);
    d->webView->load(QUrl::fromLocalFile(targetPage));

    ParameterManager *mgr = p_rd->GetParameterManager();
    connect(mgr, SIGNAL(parameterChanged(QString, QVariant)), this,
            SLOT(parameterChanged(QString, QVariant)));
}

void HtmlRunDelegate::setup()
{
    d->webView->runJavaScript(
        QL1S("var api = new HtmlApi('ws://127.0.0.1:") +
        QString::number(d->webSocketServer->serverPort()) + QL1S("', '") +
        d->webSocketServer->csrfToken() + QL1S("');"));
    d->webView->runJavaScript("setTimeout(function() { if (typeof initialize "
                              "=== 'function') initialize(); }, 1500);");
    d->hasFinishedLoading = true;
}

void HtmlRunDelegate::parameterChanged(QString name, QVariant value)
{
    QVariantList arguments;
    arguments << QVariant(name) << value;
    d->webSocketServer->broadcastMessage(WebSocketServer::buildInvokeMessage(
        QSL("parameterChanged"), arguments));
}

HtmlRunDelegate::~HtmlRunDelegate()
{
    delete d;
}

void HtmlRunDelegate::feedBack(const FeedbackMode &mode)
{
    Q_UNUSED(mode);
}
}
}
