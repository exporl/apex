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

#include "webviewwidget.h"
#include "common/global.h"

#include <QMainWindow>
#include <QString>
#include <QUrl>
#include <QVariant>
#include <QWidget>

namespace cmn
{

class WebViewPrivate;

class COMMON_EXPORT WebView : public QMainWindow
{
    Q_OBJECT
public:
    WebView();
    virtual ~WebView();

    void load(const QUrl &url);
    void loadHtml(const QString &html);
    void runJavaScript(const QString &script);
    void runJavaScriptAndEmitResult(const QString &script);
    WebViewWidget *getWebViewWidget();

Q_SIGNALS:
    void hidden();
    void loadingFinished(bool ok);
    void javaScriptFinished(const QVariant &result);

protected:
    void closeEvent(QCloseEvent *event);

private:
    DECLARE_PRIVATE(WebView)
protected:
    DECLARE_PRIVATE_DATA(WebView)
};
}
#endif
