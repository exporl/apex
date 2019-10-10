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
#include <QObject>

class QWebChannel;
class QWebSocketServer;
class WebSocketClientWrapper;

namespace cmn
{

class COMMON_EXPORT WebSocketServer : public QObject
{
    Q_OBJECT

public:
    WebSocketServer(QObject *api, const QString &name,
                    const QHostAddress &address = QHostAddress::LocalHost,
                    quint16 port = 0);
    virtual ~WebSocketServer();
    quint16 serverPort();

private:
    QScopedPointer<QWebSocketServer> webSocketServer;
    QScopedPointer<WebSocketClientWrapper> webSocketClientWrapper;
    QScopedPointer<QWebChannel> webChannel;
    void stop();
};
}

#endif
