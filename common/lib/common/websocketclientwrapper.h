#ifndef _EXPORL_COMMON_LIB_COMMON_WEBSOCKETCLIENTWRAPPER_H_
#define _EXPORL_COMMON_LIB_COMMON_WEBSOCKETCLIENTWRAPPER_H_

#include "websockettransport.h"

#include <QObject>

class QWebSocketServer;

class WebSocketClientWrapper : public QObject
{
    Q_OBJECT

public:
    WebSocketClientWrapper(QWebSocketServer *server, QObject *parent = nullptr);

signals:
    void clientConnected(WebSocketTransport *client);

private slots:
    void handleNewConnection();

private:
    QWebSocketServer *server;
};

#endif