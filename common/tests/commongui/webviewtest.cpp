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
#include <QTemporaryDir>

using namespace cmn;

class DummyObject : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE void start()
    {
        emit DummyObject::started();
    }

    Q_INVOKABLE void acknowledgeSignal()
    {
        emit DummyObject::finished();
    }

signals:
    void started();
    void signalled();
    void finished();
};

void CommonGuiTest::webViewTest()
{
    TEST_EXCEPTIONS_TRY

    WebView webView;
    webView.loadHtml(QL1S("<html><head><script>") +
                     QL1S("function gimme5() { return 5; }") +
                     QL1S("</script></head></html>"));
    QVERIFY(QSignalSpy(&webView, &WebView::loadingFinished).wait());

    connect(&webView, &WebView::javaScriptFinished,
            [](const QVariant &result) { QCOMPARE(result, QVariant(5)); });

    webView.runJavaScriptAndEmitResult(QSL("gimme5()"));
    QVERIFY(QSignalSpy(&webView, &WebView::javaScriptFinished).wait());

    TEST_EXCEPTIONS_CATCH
}

void CommonGuiTest::webViewWebSocketsTest()
{
    TEST_EXCEPTIONS_TRY

    QTemporaryDir temporaryDirectory;
    QFile::copy(
        QSL(":/qtwebchannel/qwebchannel.js"),
        QDir(temporaryDirectory.path()).filePath(QSL("qwebchannel.js")));
    QFile::copy(
        Paths::searchFile(QSL("testdata/websocket-test.html"),
                          Paths::dataDirectories()),
        QDir(temporaryDirectory.path()).filePath(QSL("websocket-test.html")));

    DummyObject dummy;

    WebSocketServer wsServer(&dummy, QSL("api"), QHostAddress::LocalHost, 3456);

    WebView webView;
    webView.load(QUrl::fromLocalFile(
        QDir(temporaryDirectory.path()).filePath(QSL("websocket-test.html"))));
    webView.show();

    QVERIFY(QSignalSpy(&dummy, &DummyObject::started).wait());

    emit dummy.signalled();

    QVERIFY(QSignalSpy(&dummy, &DummyObject::finished).wait());

    TEST_EXCEPTIONS_CATCH
}

QTEST_MAIN(CommonGuiTest)

#include "webviewtest.moc"