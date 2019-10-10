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

#if defined(Q_OS_ANDROID)
#include <QQuickItem>
#include <QQuickWidget>
#else
#include <QWebEngineView>
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

    void load(const QUrl &url);
    void loadHtml(const QString &html, const QUrl &baseUrl = QUrl());
    void runJavaScript(const QString &script);
    void runJavaScriptAndEmitResult(const QString &script);

/* Provided for custom cases, but use sparingly */
#if defined(Q_OS_ANDROID)
    QQuickWidget *webView();
#else
    QWebEngineView *webView();
#endif

Q_SIGNALS:
    void hidden();
    void loadingFinished(bool ok = true);
    void javaScriptFinished(const QVariant &result);

protected:
    void closeEvent(QCloseEvent *event);

private:
    DECLARE_PRIVATE(WebView)
protected:
    DECLARE_PRIVATE_DATA(WebView)
private Q_SLOTS:
    void preventMultipleLoadingFinishedSignals();
};
}
#endif
