#include "websocketclientwrapper.h"

#include <QWebSocketServer>

WebSocketClientWrapper::WebSocketClientWrapper(QWebSocketServer *server,
                                               QObject *parent)
    : QObject(parent), server(server)
{
    connect(server, &QWebSocketServer::newConnection, this,
            &WebSocketClientWrapper::handleNewConnection);
}

void WebSocketClientWrapper::handleNewConnection()
{
    emit clientConnected(
        new WebSocketTransport(server->nextPendingConnection()));
}
