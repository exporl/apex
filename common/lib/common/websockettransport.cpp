#include "websockettransport.h"
#include "global.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QWebSocket>

WebSocketTransport::WebSocketTransport(QWebSocket *socket)
    : QWebChannelAbstractTransport(socket), socket(socket)
{
    connect(socket, &QWebSocket::textMessageReceived, this,
            &WebSocketTransport::textMessageReceived);
    connect(socket, &QWebSocket::disconnected, this,
            &WebSocketTransport::deleteLater);
}

WebSocketTransport::~WebSocketTransport()
{
    socket->deleteLater();
}

void WebSocketTransport::sendMessage(const QJsonObject &message)
{
    QJsonDocument doc(message);
    socket->sendTextMessage(
        QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
}

void WebSocketTransport::textMessageReceived(const QString &messageData)
{
    QJsonParseError error;
    QJsonDocument message =
        QJsonDocument::fromJson(messageData.toUtf8(), &error);
    if (error.error) {
        qCWarning(EXPORL_COMMON)
            << "Failed to parse text message as JSON object:" << messageData
            << "Error is:" << error.errorString();
        return;
    } else if (!message.isObject()) {
        qCWarning(EXPORL_COMMON)
            << "Received JSON message that is not an object: " << messageData;
        return;
    }
    emit messageReceived(message.object(), this);
}