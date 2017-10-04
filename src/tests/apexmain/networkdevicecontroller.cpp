/******************************************************************************
 * Copyright (C) 2017 Sanne Raymaekers <sanne.raymaekers@gmail.com>           *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
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

#include "apexmaintest.h"

#include "apexmain/device/plugincontrollerinterface.h"

#include "common/global.h"
#include "common/pluginloader.h"
#include "common/testutils.h"

#include <QTcpServer>

using namespace cmn;

void ApexMainTest::testNetworkDeviceController()
{
    TEST_EXCEPTIONS_TRY

    PluginControllerCreator *controllerCreator = NULL;
    Q_FOREACH (PluginControllerCreator *creator,
               PluginLoader().availablePlugins<PluginControllerCreator>()) {
        if (creator->availablePlugins().contains(
                QLatin1String("networkdevicecontroller"))) {
            controllerCreator = creator;
            break;
        }
    }

    QScopedPointer<PluginControllerInterface> controller(
        controllerCreator->createController(QSL("networkdevicecontroller")));

    QTcpServer server;
    server.listen(QHostAddress::LocalHost);
    QTcpSocket *socket;

    connect(&server, &QTcpServer::newConnection, [&]() {
        socket = server.nextPendingConnection();
        connect(socket, &QTcpSocket::readyRead, [&]() {
            QString readLine = QString::fromUtf8(socket->readLine());
            if (readLine.trimmed() == QSL("Start|filename|subject|note")) {
                QString message = QSL("Recording started\r\n");
                socket->write(message.toUtf8());
            } else if (readLine.trimmed() == QSL("Stop")) {
                QString message = QSL("Recording stopped\r\n");
                socket->write(message.toUtf8());
            } else {
                QFAIL("Unexpected message received from controller " +
                      readLine.toUtf8());
            }
        });
    });

    controller->setParameter(QSL("host"), -1, QSL("127.0.0.1"));
    controller->setParameter(QSL("port"), -1,
                             QString::number(server.serverPort()));
    controller->setParameter(QSL("filename"), -1, QSL("filename"));
    controller->setParameter(QSL("subject"), -1, QSL("subject"));
    controller->setParameter(QSL("note"), -1, QSL("note"));
    QVERIFY(controller->prepare());
    QTest::qWait(250);
    QSignalSpy spy(socket, SIGNAL(disconnected()));
    controller->release();
    QVERIFY(spy.wait(500));

    TEST_EXCEPTIONS_CATCH
}
