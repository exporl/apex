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
#include <QScopedPointer>
#include <QTimer>

#if !defined(Q_OS_ANDROID)
#include <QWebEngineSettings>
#endif

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

public:
#if defined(Q_OS_ANDROID)
    QQuickWidget *webView;
#else
    QWebEngineView *webView;
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

#if defined(Q_OS_ANDROID)
    d->webView = new QQuickWidget(this);
    d->webView->setResizeMode(QQuickWidget::SizeRootObjectToView);
    d->webView->setSource(QSL("qrc:/commongui/webview.qml"));
    connect(d->webView->rootObject(), SIGNAL(loadingFinished()), this,
            SIGNAL(loadingFinished()));
    connect(this, SIGNAL(loadingFinished()), this,
            SLOT(preventMultipleLoadingFinishedSignals()));
    connect(d->webView->rootObject(), SIGNAL(javascriptFinished(QVariant)),
            this, SIGNAL(javaScriptFinished(QVariant)));
#else
    d->webView = new QWebEngineView(this);
    d->webView->settings()->setAttribute(
        QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);

    connect(d->webView, &QWebEngineView::loadFinished, this,
            &WebView::loadingFinished);
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
#if defined(Q_OS_ANDROID)
    Q_UNUSED(baseUrl);
    QMetaObject::invokeMethod(d->webView->rootObject(), "loadHtml",
                              Q_ARG(QVariant, html));
#else
    d->webView->setHtml(html, baseUrl);
#endif
}

void WebView::load(const QUrl &url)
{
    E_D(WebView);
#if defined(Q_OS_ANDROID)
    QMetaObject::invokeMethod(d->webView->rootObject(), "load",
                              Q_ARG(QVariant, url));
#else
    d->webView->load(url);
#endif
}

void WebView::runJavaScript(const QString &script)
{
    E_D(WebView);
#if defined(Q_OS_ANDROID)
    QMetaObject::invokeMethod(d->webView->rootObject(), "runJavaScript",
                              Q_ARG(QVariant, script));
#else
    d->webView->page()->runJavaScript(script);
#endif
}

void WebView::runJavaScriptAndEmitResult(const QString &script)
{
    E_D(WebView);
#if defined(Q_OS_ANDROID)
    QMetaObject::invokeMethod(d->webView->rootObject(),
                              "runJavaScriptAndEmitResult",
                              Q_ARG(QVariant, script));
#else
    d->webView->page()->runJavaScript(
        script,
        [this, script](const QVariant &v) { emit javaScriptFinished(v); });
#endif
}

#if defined(Q_OS_ANDROID)
QQuickWidget
#else
QWebEngineView
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

// prevent multiple loadingFinished signals
// (caused by loading jquery-mobile)
void WebView::preventMultipleLoadingFinishedSignals()
{
#if defined(ANDROID)
    E_D(WebView);
    disconnect(d->webView->rootObject(), SIGNAL(loadingFinished()), this,
               SIGNAL(loadingFinished()));
#endif
}
}

#include "webview.moc"
