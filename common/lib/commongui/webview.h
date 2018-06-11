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

#ifndef _EXPORL_COMMON_LIB_COMMONGUI_WEBVIEW_H_
#define _EXPORL_COMMON_LIB_COMMONGUI_WEBVIEW_H_

#include "common/global.h"

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QObject>
#include <QUrl>
#include <QVariant>

#if defined(ANDROID)
#include <QQuickItem>
#include <QQuickWidget>
#elif defined(WITH_WEBENGINE)
#include <QWebEngineView>
#else
#include <QWebFrame>
#include <QWebView>
#endif

namespace cmn
{

class WebViewPrivate;

class COMMON_EXPORT WebView : public QMainWindow
{
    Q_OBJECT
public:
    WebView();
    ~WebView();

    /* WebView::load is probably not what you need on android, since loading
     * content from a resource package does not work.
     * Use WebView::loadHtml instead.
     */
    void load(const QUrl &url);
    void loadHtml(const QString &html, const QUrl &baseUrl = QUrl());
    /* Is not blocking with QWebEngine. Always returns default-constructed
     * QVariant.
     */
    QVariant runJavaScript(const QString &script, int timeout = 5000);

    /* Does not work on android */
    void addToJavaScriptWindowObject(const QString &name, QObject *object);
    /* Does not work on android */
    void setNetworkAccessManager(QNetworkAccessManager *accessManager);

#if defined(ANDROID)
    QQuickWidget
#elif defined(WITH_WEBENGINE)
    QWebEngineView
#else
    QWebView
#endif
        /* Provided for custom cases, but use sparingly */
        *
        webView();

Q_SIGNALS:
    void hidden();
    void loadingFinished(bool = true);

protected:
    void closeEvent(QCloseEvent *event);

private:
    DECLARE_PRIVATE(WebView)
protected:
    DECLARE_PRIVATE_DATA(WebView)
};
}
#endif
