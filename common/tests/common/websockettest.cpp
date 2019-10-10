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

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSignalSpy>
#include <QWebSocket>

using namespace cmn;

class DummyObject : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE QString dummyMethod(const QString &input)
    {
        return input;
    }
};

void CommonTest::webSocketTest()
{
    TEST_EXCEPTIONS_TRY

    DummyObject dummy;

    WebSocketServer wsServer(&dummy, QSL("webSocketTest"));

    QWebSocket webSocket;
    connect(&webSocket, &QWebSocket::textMessageReceived,
            [](const QString &message) {
                QJsonObject actual =
                    QJsonDocument::fromJson(message.toUtf8()).object();
                QCOMPARE(actual.value("data").toString(), QString("expected"));
            });

    connect(&webSocket, &QWebSocket::connected, [&webSocket]() {
        QJsonObject invokeObject;
        invokeObject.insert(QString("type"), QJsonValue(6)); // 6: invokeMethod
        invokeObject.insert(QString("object"),
                            QJsonValue(QString("webSocketTest")));
        invokeObject.insert(
            QString("method"),
            QJsonValue(5)); // 5: dummyMethod TODO: make this more robust
        invokeObject.insert(
            QString("args"),
            QJsonValue(QJsonArray::fromStringList({"expected"})));
        invokeObject.insert(QString("id"), QJsonValue(1));
        webSocket.sendTextMessage(
            QString::fromUtf8(QJsonDocument(invokeObject).toJson()));
    });

    webSocket.open(QSL("ws://127.0.0.1:%1").arg(wsServer.serverPort()));

    QVERIFY(QSignalSpy(&webSocket, &QWebSocket::textMessageReceived).wait());

    TEST_EXCEPTIONS_CATCH
}

#include "websockettest.moc"