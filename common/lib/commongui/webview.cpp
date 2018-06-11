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
#include <QApplication>
#include <QEventLoop>
#include <QMenu>
#include <QMenuBar>
#include <QMetaObject>
#include <QMutex>
#include <QScopedPointer>
#include <QTimer>

#if defined(WITH_WEBENGINE)
#include <QWebEngineSettings>
#endif

namespace cmn
{

class WebViewPrivate : public QObject
{
    Q_OBJECT

public:
    WebViewPrivate(QObject *parent) : QObject(parent), callbacknr(0)
    {
    }
    ~WebViewPrivate()
    {
    }

public Q_SLOTS:
    void javascriptFinished(const QVariant &result)
    {
        lastJavascriptResult = result;
        Q_EMIT receivedJavascript();
    }

Q_SIGNALS:
    void receivedJavascript();

public:
    QMutex mutex;
    int progress;
    int callbacknr;
    QVariant lastJavascriptResult;
#if defined(ANDROID)
    QQuickWidget *webView;
#elif defined(WITH_WEBENGINE)
    QWebEngineView *webView;
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
    connect(d->webView->rootObject(), SIGNAL(javascriptFinished(QVariant)), d,
            SLOT(javascriptFinished(QVariant)));
#elif defined(WITH_WEBENGINE)
    d->webView = new QWebEngineView(this);
    d->webView->settings()->setAttribute(
        QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    connect(d->webView, SIGNAL(loadFinished(bool)), this,
            SIGNAL(loadingFinished(bool)));
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
#elif defined(WITH_WEBENGINE)
    d->webView->setHtml(html, baseUrl);
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
#elif defined(WITH_WEBENGINE)
    d->webView->load(url);
#else
    d->webView->page()->mainFrame()->load(url);
#endif
}

QVariant WebView::runJavaScript(const QString &script, int timeout)
{
    E_D(WebView);
#if defined(ANDROID)
    QEventLoop loop;
    QTimer timer;
    QMetaObject::invokeMethod(d->webView->rootObject(), "runJavaScript",
                              Q_ARG(QVariant, script));
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    connect(d, SIGNAL(receivedJavascript()), &loop, SLOT(quit()));
    timer.start(timeout);
    loop.exec();
    return d->lastJavascriptResult;
#elif defined(WITH_WEBENGINE)
    Q_UNUSED(timeout);
    d->webView->page()->runJavaScript(script);
    return QVariant();
#else
    Q_UNUSED(timeout);
    return d->webView->page()->mainFrame()->evaluateJavaScript(script);
#endif
}

void WebView::addToJavaScriptWindowObject(const QString &name, QObject *object)
{
#if defined(ANDROID) || defined(WITH_WEBENGINE)
    Q_UNUSED(name);
    Q_UNUSED(object);
#else
    E_D(WebView);
    d->webView->page()->mainFrame()->addToJavaScriptWindowObject(name, object);
#endif
}

void WebView::setNetworkAccessManager(QNetworkAccessManager *accessManager)
{
#if defined(ANDROID) || defined(WITH_WEBENGINE)
    Q_UNUSED(accessManager);
#else
    E_D(WebView);
    d->webView->page()->setNetworkAccessManager(accessManager);
#endif
}

#if defined(ANDROID)
QQuickWidget
#elif defined(WITH_WEBENGINE)
QWebEngineView
#else
QWebView
#endif
*WebView::webView()
{
    E_D(WebView);
    return d->webView;
}

void WebView::closeEvent(QCloseEvent *event)
{
    Q_EMIT hidden();
    QWidget::closeEvent(event);
}
}

#include "webview.moc"
