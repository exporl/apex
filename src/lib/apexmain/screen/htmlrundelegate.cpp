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
    QScopedPointer<HtmlAPI> api;
    QScopedPointer<WebSocketServer> webSocketServer;
    QScopedPointer<WebView> webView;
    QString lastAnswer;
    QTemporaryDir temporaryDirectory;

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
    if (enable)
        this->enable();
    ScreenElementRunDelegate::setEnabled(enable);
}

void HtmlRunDelegate::enable()
{
    QString code;
    if (!d->hasFinishedLoading)
        code = QSL("setTimeout(function() { enabled(); }, 1000);");
    else
        code = QSL("enabled();");
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
    return d->lastAnswer;
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

void HtmlRunDelegate::collectAnswer(const QString &answer)
{
    d->lastAnswer = answer;
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
        Paths::searchDirectory(QSL("resultsviewer/external"),
                               Paths::dataDirectories()),
        d->temporaryDirectory.path());
    QFile::copy(
        ":/qtwebchannel/qwebchannel.js",
        QDir(d->temporaryDirectory.path()).filePath(QSL("qwebchannel.js")));

    d->api.reset(new HtmlAPI());
    connect(d->api.data(), &HtmlAPI::answered, this,
            &HtmlRunDelegate::collectAnswer);

    d->webSocketServer.reset(
        new WebSocketServer(d->api.data(), QSL("htmlApi")));

    d->webView.reset(new WebView());
    connect(d->webView.data(), &WebView::loadingFinished,
            [this]() { d->hasFinishedLoading = true; });

    QUrl sourcePage = AccessManager::prepare(element->page());
    QString htmlPath =
        ApexTools::copyAndPrepareAsHtmlFileWithInjectedBootstrapValues(
            AccessManager::prepare(sourcePage).toLocalFile(),
            d->temporaryDirectory.path(), d->webSocketServer->serverPort());
    d->webView->load(QUrl::fromLocalFile(htmlPath));

    connect(p_rd->GetParameterManager(), &ParameterManager::parameterChanged,
            d->api.data(), &HtmlAPI::parameterChanged);
}

HtmlRunDelegate::~HtmlRunDelegate()
{
    delete d;
}

void HtmlRunDelegate::feedBack(const FeedbackMode &mode)
{
    Q_UNUSED(mode);
}
} // namespace rundelegates
} // namespace apex
