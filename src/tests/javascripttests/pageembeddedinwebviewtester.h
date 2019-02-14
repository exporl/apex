#ifndef _APEX_SRC_TESTS_JAVASCRIPTTESTS_PAGEEMBEDDEDINWEBVIEWTESTER_H_
#define _APEX_SRC_TESTS_JAVASCRIPTTESTS_PAGEEMBEDDEDINWEBVIEWTESTER_H_

#include "commongui/webview.h"
#include <QObject>
#include <QWebSocketServer>

class PageEmbeddedInWebviewTester : public QObject
{
    Q_OBJECT

public:
    PageEmbeddedInWebviewTester(const QUrl &url);

signals:
    void testResultsCollected(const QString &message);

private:
    QScopedPointer<QWebSocketServer> webSocketServer;
    QScopedPointer<cmn::WebView> webView;

private slots:
    void newConnection();
    void injectCollectorScript();
};

#endif