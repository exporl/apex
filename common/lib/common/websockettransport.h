#ifndef _EXPORL_COMMON_LIB_COMMON_WEBSOCKETTRANSPORT_H_
#define _EXPORL_COMMON_LIB_COMMON_WEBSOCKETTRANSPORT_H_

#include <QWebChannelAbstractTransport>

class QWebSocket;

class WebSocketTransport : public QWebChannelAbstractTransport
{
    Q_OBJECT
public:
    explicit WebSocketTransport(QWebSocket *socket);
    virtual ~WebSocketTransport();

    void sendMessage(const QJsonObject &message) override;

private slots:
    void textMessageReceived(const QString &message);

private:
    QWebSocket *socket;
};

#endif
