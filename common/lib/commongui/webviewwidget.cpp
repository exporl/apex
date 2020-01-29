#include "webviewwidget.h"

#include <QQuickItem>

namespace cmn
{

WebViewWidget::WebViewWidget(QWidget *parent) : QQuickWidget(parent)
{
    setResizeMode(QQuickWidget::SizeRootObjectToView);
    setSource(QSL("qrc:/commongui/webview.qml"));
    connect(rootObject(), SIGNAL(loadingFinished(bool)), this,
            SIGNAL(loadingFinished(bool)));
    connect(rootObject(), SIGNAL(javaScriptFinished(QVariant)), this,
            SIGNAL(javaScriptFinished(QVariant)));
    connect(this, SIGNAL(loadingFinished(bool)), this,
            SLOT(preventMultipleLoadingFinishedSignals()));
}

void WebViewWidget::loadHtml(const QString &html)
{
    QMetaObject::invokeMethod(rootObject(), "loadHtml", Q_ARG(QVariant, html));
}

void WebViewWidget::loadUrl(const QUrl &url)
{
    QMetaObject::invokeMethod(rootObject(), "load", Q_ARG(QVariant, url));
}

void WebViewWidget::runJavaScript(const QString &script)
{
    QMetaObject::invokeMethod(rootObject(), "runJavaScript",
                              Q_ARG(QVariant, script));
}

void WebViewWidget::runJavaScriptAndEmitResult(const QString &script)
{
    QMetaObject::invokeMethod(rootObject(), "runJavaScriptAndEmitResult",
                              Q_ARG(QVariant, script));
}

void WebViewWidget::print()
{
    runJavaScript(QSL("window.print();"));
}

void WebViewWidget::preventMultipleLoadingFinishedSignals()
{
    disconnect(rootObject(), SIGNAL(loadingFinished(bool)), this,
               SIGNAL(loadingFinished(bool)));
}
}
