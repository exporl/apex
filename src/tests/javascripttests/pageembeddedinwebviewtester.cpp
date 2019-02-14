#include <QWebSocket>
#include <QWebSocketServer>

#include "commongui/webview.h"

#include "pageembeddedinwebviewtester.h"

PageEmbeddedInWebviewTester::PageEmbeddedInWebviewTester(const QUrl &url)
{
    webSocketServer.reset(
        new QWebSocketServer(NULL, QWebSocketServer::NonSecureMode, this));
    webSocketServer->listen();
    connect(webSocketServer.data(), SIGNAL(newConnection()), this,
            SLOT(newConnection()));

    webView.reset(new cmn::WebView());
    webView->load(url);
    connect(webView.data(), SIGNAL(loadingFinished()), this,
            SLOT(injectCollectorScript()));
}

void PageEmbeddedInWebviewTester::injectCollectorScript()
{
    QString injectedScript =
        "(function() {\n"
        "  allDonePromise.then(function(result) {\n"
        "    var webSocket = new WebSocket('ws://127.0.0.1:%1');\n"
        "    webSocket.onopen = function() {\n"
        "      webSocket.send(result);\n"
        "    };\n"
        "  });\n"
        "})();";

    webView->runJavaScript(injectedScript.arg(webSocketServer->serverPort()));
}

void PageEmbeddedInWebviewTester::newConnection()
{
    QWebSocket *newSocket = webSocketServer->nextPendingConnection();
    connect(newSocket, SIGNAL(textMessageReceived(QString)), this,
            SIGNAL(testResultsCollected(QString)));
    connect(newSocket, SIGNAL(textMessageReceived()), newSocket,
            SIGNAL(close()));
    connect(newSocket, SIGNAL(textMessageReceived()), newSocket,
            SIGNAL(deleteLater()));
}
