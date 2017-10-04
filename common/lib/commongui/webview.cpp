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

#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QMetaObject>
#include <QScopedPointer>

namespace cmn
{

class WebViewPrivate : public QObject
{
    Q_OBJECT

public:
    WebViewPrivate(QObject *parent) : QObject(parent)
    {
    }
    ~WebViewPrivate()
    {
    }
#if defined(ANDROID)
    QQuickWidget *webView;
#else
    QWebView *webView;
#endif
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

#if defined(ANDROID)
    d->webView = new QQuickWidget(this);
    d->webView->setResizeMode(QQuickWidget::SizeRootObjectToView);
    d->webView->setSource(QSL("qrc:/commongui/webview.qml"));
    connect(d->webView->rootObject(), SIGNAL(loadingFinished()), this,
            SIGNAL(loadingFinished()));
    connect(d->webView->rootObject(), SIGNAL(javascriptFinished(QVariant)),
            this, SIGNAL(javascriptFinished(QVariant)));
#else
    QWebSettings::globalSettings()->setAttribute(
        QWebSettings::DeveloperExtrasEnabled, true);
    QWebSettings::globalSettings()->setAttribute(
        QWebSettings::LocalStorageEnabled, true);
    d->webView = new QWebView(this);
    connect(d->webView, SIGNAL(loadFinished(bool)), this,
            SIGNAL(loadingFinished(bool)));
#endif
    setCentralWidget(d->webView);
}

WebView::~WebView()
{
    delete dataPtr;
}

void WebView::loadHtml(const QString &html, const QUrl &baseUrl)
{
    E_D(WebView);
#if defined(ANDROID)
    Q_UNUSED(baseUrl);
    QMetaObject::invokeMethod(d->webView->rootObject(), "loadHtml",
                              Q_ARG(QVariant, html));
#else
    d->webView->page()->mainFrame()->setHtml(html, baseUrl);
#endif
}

void WebView::load(const QUrl &url)
{
    E_D(WebView);
#if defined(ANDROID)
    QMetaObject::invokeMethod(d->webView->rootObject(), "load",
                              Q_ARG(QVariant, url));
#else
    d->webView->page()->mainFrame()->load(url);
#endif
}

void WebView::runJavaScript(const QString &script)
{
    E_D(WebView);
#if defined(ANDROID)
    QMetaObject::invokeMethod(d->webView->rootObject(), "runJavaScript",
                              Q_ARG(QVariant, script));
#else
    Q_EMIT javascriptFinished(
        d->webView->page()->mainFrame()->evaluateJavaScript(script));
#endif
}

void WebView::addToJavaScriptWindowObject(const QString &name, QObject *object)
{
#if defined(ANDROID)
    Q_UNUSED(name);
    Q_UNUSED(object);
#else
    E_D(WebView);
    d->webView->page()->mainFrame()->addToJavaScriptWindowObject(name, object);
#endif
}

void WebView::setNetworkAccessManager(QNetworkAccessManager *accessManager)
{
#if defined(ANDROID)
    Q_UNUSED(accessManager);
#else
    E_D(WebView);
    d->webView->page()->setNetworkAccessManager(accessManager);
#endif
}

#if defined(ANDROID)
QQuickWidget
#else
QWebView
#endif
*WebView::webView()
{
    E_D(WebView);
#if defined(ANDROID)
    return d->webView;
#else
    return d->webView;
#endif
}

void WebView::closeEvent(QCloseEvent *event)
{
    Q_EMIT hidden();
    QWidget::closeEvent(event);
}
}

#include "webview.moc"
