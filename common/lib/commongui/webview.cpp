/******************************************************************************
 * Copyright (C) 2017 Sanne Raymaekers <sanne.raymaekers@gmail.com>           *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 3 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with this program; if not, write to the Free Software Foundation, Inc.,    *
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                *
 ******************************************************************************/

#include "webview.h"
#include "webviewwidget.h"

#include <QAction>
#include <QApplication>
#include <QEventLoop>
#include <QMenu>
#include <QMenuBar>
#include <QMetaObject>
#include <QScopedPointer>
#include <QTimer>
#include <QWidget>

namespace cmn
{

class WebViewPrivate : public QObject
{
    Q_OBJECT

public:
    WebViewPrivate(QObject *parent) : QObject(parent)
    {
    }
    virtual ~WebViewPrivate()
    {
        delete webViewWidget;
    }

public:
    WebViewWidget *webViewWidget;
};

WebView::WebView() : dataPtr(new WebViewPrivate(this))
{
    E_D(WebView);
    setAttribute(Qt::WA_DeleteOnClose, false);
    setAttribute(Qt::WA_QuitOnClose, false);

    QMenu *fileMenu = new QMenu(tr("&File"), menuBar());
    QAction *hideWindowAction = new QAction(tr("Hide"), fileMenu);
    connect(hideWindowAction, SIGNAL(triggered()), this, SLOT(hide()));
    connect(hideWindowAction, SIGNAL(triggered()), this, SIGNAL(hidden()));
    fileMenu->addAction(hideWindowAction);
    menuBar()->addMenu(fileMenu);

    d->webViewWidget = new WebViewWidget(this);
    connect(d->webViewWidget, &WebViewWidget::javaScriptFinished, this,
            &WebView::javaScriptFinished);
    connect(d->webViewWidget, &WebViewWidget::loadingFinished, this,
            &WebView::loadingFinished);
    setCentralWidget(d->webViewWidget);
}

WebView::~WebView()
{
    delete dataPtr;
}

void WebView::loadHtml(const QString &html)
{
    E_D(WebView);
    d->webViewWidget->loadHtml(html);
}

void WebView::load(const QUrl &url)
{
    E_D(WebView);
    d->webViewWidget->loadUrl(url);
}

void WebView::runJavaScript(const QString &script)
{
    E_D(WebView);
    d->webViewWidget->runJavaScript(script);
}

void WebView::runJavaScriptAndEmitResult(const QString &script)
{
    E_D(WebView);
    d->webViewWidget->runJavaScriptAndEmitResult(script);
}

WebViewWidget *WebView::getWebViewWidget()
{
    E_D(WebView);
    return d->webViewWidget;
}

void WebView::closeEvent(QCloseEvent *event)
{
    Q_EMIT hidden();
    QWidget::closeEvent(event);
}
}

#include "webview.moc"
