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

#include "websocketserver.h"
#include "exception.h"
#include "websocketclientwrapper.h"

#include <QWebChannel>
#include <QWebSocketServer>

namespace cmn
{

WebSocketServer::WebSocketServer(QObject *api, const QString &name,
                                 const QHostAddress &address, quint16 port)
    : webSocketServer(
          new QWebSocketServer(name, QWebSocketServer::NonSecureMode, this)),
      webSocketClientWrapper(
          new WebSocketClientWrapper(webSocketServer.data())),
      webChannel(new QWebChannel)
{
    if (!webSocketServer->listen(address, port))
        throw Exception(tr("Couldn't start listening at %s:%d")
                            .arg(webSocketServer->serverAddress().toString())
                            .arg(webSocketServer->serverPort()));

    connect(webSocketClientWrapper.data(),
            &WebSocketClientWrapper::clientConnected, webChannel.data(),
            &QWebChannel::connectTo);

    webChannel->registerObject(name, api);
}

WebSocketServer::~WebSocketServer()
{
    stop();
}

void WebSocketServer::stop()
{
    if (webSocketServer->isListening())
        webSocketServer->close();
}

quint16 WebSocketServer::serverPort()
{
    return webSocketServer->serverPort();
}
}
