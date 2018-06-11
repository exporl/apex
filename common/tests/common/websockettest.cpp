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

#include "commontest.h"

#include "common/testutils.h"
#include "common/websocketserver.h"

#include <QJsonDocument>
#include <QSignalSpy>
#include <QTest>
#include <QWebSocket>

using namespace cmn;

void CommonTest::webSocketTest()
{
    TEST_EXCEPTIONS_TRY

    DummyQObject *dummy = new DummyQObject;
    QWebSocket webSocket;
    QSignalSpy spy(dummy, SIGNAL(dummySignal()));
    WebSocketServer wsServer(QSL("dummy"));

    wsServer.on(QSL("concatSlot"), dummy, QSL("concatSlot(QString,int)"));
    wsServer.start(QHostAddress::LocalHost);

    connect(&webSocket, &QWebSocket::textMessageReceived,
            [](const QString &message) {
                QJsonObject messageObject =
                    QJsonDocument::fromJson(message.toUtf8()).object();
                QCOMPARE(messageObject.value(QSL("type")).toString(),
                         QL1S("return"));
                QCOMPARE(messageObject.value(QSL("data")).toString(),
                         QL1S("dummyarg10"));
            });

    connect(&webSocket, &QWebSocket::connected, [&wsServer, &webSocket]() {
        QVariantList args;
        args.append(QVariant(QSL("dummyarg")));
        args.append(QVariant(10));
        QJsonObject invokeObject =
            WebSocketServer::buildInvokeMessage(QSL("concatSlot"), args);
        invokeObject.insert(QSL("token"), QJsonValue(wsServer.csrfToken()));
        webSocket.sendTextMessage(
            QString::fromUtf8(QJsonDocument(invokeObject).toJson()));
    });

    webSocket.open(QL1S("ws://127.0.0.1:") +
                   QString::number(wsServer.serverPort()));
    QTest::qWait(250);
    QVERIFY(spy.count() == 1);
    wsServer.stop();

    TEST_EXCEPTIONS_CATCH
}
