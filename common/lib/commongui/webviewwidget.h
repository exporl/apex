#ifndef _EXPORL_COMMON_LIB_COMMONGUI_WEBVIEWWIDGET_H_
#define _EXPORL_COMMON_LIB_COMMONGUI_WEBVIEWWIDGET_H_

#include "common/global.h"

#include <QString>
#include <QUrl>

#include <QQuickWidget>

namespace cmn
{

class COMMON_EXPORT WebViewWidget : public QQuickWidget
{
    Q_OBJECT
public:
    WebViewWidget(QWidget *parent = nullptr);

    void loadHtml(const QString &html);
    void loadUrl(const QUrl &url);
    void runJavaScript(const QString &script);
    void runJavaScriptAndEmitResult(const QString &script);
    void print();

signals:
    void loadingFinished(bool ok);
    void javaScriptFinished(const QVariant &result);

private slots:
    // prevent multiple loadingFinished signals (caused by loading
    // jquery-mobile)
    void preventMultipleLoadingFinishedSignals();
};
}
#endif
