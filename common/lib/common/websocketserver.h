/******************************************************************************
 * Copyright (C) 2017  Sanne Raymaekers <sanne.raymaekers@gmail.com>          *
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

#ifndef _EXPORL_COMMON_LIB_COMMON_WEBSOCKETSERVER_H_
#define _EXPORL_COMMON_LIB_COMMON_WEBSOCKETSERVER_H_

#include "common/global.h"

#include <QHostAddress>
#include <QJsonObject>
#include <QObject>

namespace cmn
{

class WebSocketServerPrivate;

class COMMON_EXPORT WebSocketServer : public QObject
{
    Q_OBJECT

public:
    WebSocketServer(const QString &serverName);

    virtual ~WebSocketServer();

    quint16 serverPort();
    QString csrfToken();

    void on(const QString &method, QObject *object, const QString &slot);
    void removeListener(const QString &method, QObject *object,
                        const QString &slot);
    void removeAllListeners(const QString &method);

    static QJsonObject buildUserMessage(const QString &message);
    static QJsonObject buildInvokeMessage(const QString &method,
                                          const QVariantList &arguments);
public Q_SLOTS:
    void broadcastMessage(const QJsonObject &message);
    void start(const QHostAddress &address = QHostAddress::LocalHost,
               quint16 port = 0);
    void stop();

Q_SIGNALS:
    void messageReceived(const QJsonObject &message);
    void newConnection();

private:
    DECLARE_PRIVATE(WebSocketServer)
protected:
    DECLARE_PRIVATE_DATA(WebSocketServer)
};
}

#endif
