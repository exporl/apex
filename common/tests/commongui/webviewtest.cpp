/******************************************************************************
 * Copyright (C) 2017 Sanne Raymaekers <sanne.raymaekers@gmail.com>           *
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

#include "commonguitest.h"

#include "common/global.h"
#include "common/paths.h"
#include "common/testutils.h"
#include "common/websocketserver.h"

#include "commongui/webview.h"

#include <QSignalSpy>
#include <QString>
#include <QVariantList>

using namespace cmn;

static bool wait(const QSignalSpy &spy, unsigned duration = 50,
                 unsigned count = 20)
{
    for (unsigned i = 0; i < count && spy.count() == 0; ++i) {
        QCoreApplication::processEvents();
        QTest::qSleep(duration);
    }
    return spy.count() > 0;
}

void CommonGuiTest::webViewTest()
{
    TEST_EXCEPTIONS_TRY

    WebView webView;
    QSignalSpy spy(&webView, SIGNAL(loadingFinished(bool)));
    webView.loadHtml(
        QL1S("<html><head><script>function gimme5() { return 5; } ") +
        QL1S("</script></head></html>"));
    webView.show();
    QVERIFY(spy.count() == 1 || spy.wait());

#if !defined(WITH_WEBENGINE)
    QCOMPARE(QVariant(5),
             webView.runJavaScript(QString::fromLatin1("gimme5()")));
#endif

    TEST_EXCEPTIONS_CATCH
}

void CommonGuiTest::webViewWebSocketsTest()
{
    TEST_EXCEPTIONS_TRY

    WebSocketServer wsServer(QSL("webviewtest"));
    DummyQObject dummy;
    QSignalSpy spy(&dummy, SIGNAL(dummySignal(QString)));

    wsServer.start();
    wsServer.on(QSL("dummySlot"), &dummy, QSL("dummySlot(QString)"));
    connect(&dummy, &DummyQObject::dummySignal, [](const QString &myString) {
        QCOMPARE(myString, QSL("testString"));
    });
    QSignalSpy connectedSpy(&wsServer, SIGNAL(newConnection()));

    WebView webView;
    QSignalSpy loadingSpy(&webView, SIGNAL(loadingFinished()));
    webView.loadHtml(QSL("<html><head></head></html>"));
    QVERIFY(loadingSpy.count() == 1 || loadingSpy.wait());

    QFile polyfill(
        Paths::searchFile(QSL("js/polyfill.js"), Paths::dataDirectories()));
    QFile webSocket(Paths::searchFile(QSL("js/commonwebsocket.js"),
                                      Paths::dataDirectories()));
    polyfill.open(QIODevice::ReadOnly);
    webSocket.open(QIODevice::ReadOnly);
    webView.runJavaScript(QString::fromUtf8(polyfill.readAll()));
    webView.runJavaScript(QString::fromUtf8(webSocket.readAll()));
    webView.runJavaScript(
        QSL("var socket = new CommonSocket('ws://127.0.0.1:%1', '%2');")
            .arg(wsServer.serverPort())
            .arg(wsServer.csrfToken()) +
        QSL("socket.open();") +
        QSL("socket.on('eval', function(js) { eval(js); });"));
    QVERIFY(wait(connectedSpy));
    wsServer.broadcastMessage(WebSocketServer::buildInvokeMessage(
        QSL("eval"),
        QVariantList() << QL1S("socket.sendAsync(socket.buildInvokeMessage('") +
                              QL1S("dummySlot', ['testString']));")));
    QVERIFY(wait(spy));
    wsServer.stop();

    TEST_EXCEPTIONS_CATCH
}

QTEST_MAIN(CommonGuiTest)
