#include "javascripttests.h"
#include "pageembeddedinwebviewtester.h"
#include "common/paths.h"
#include "common/testutils.h"

#include <QtTest>

using namespace cmn;

void JavaScriptTests::testScript_data()
{
    QTest::addColumn<QString>("pathToHtmlTestFile");

    QTest::newRow("listenrunner")
        << "examples/flowrunner/test/listenrunner.test.html";
}

void JavaScriptTests::testScript()
{
#if defined(WITH_WEBENGINE)
    TEST_EXCEPTIONS_TRY

    QFETCH(QString, pathToHtmlTestFile);
    QString absolutePathToHtmlTestFile =
        Paths::searchFile(pathToHtmlTestFile, Paths::dataDirectories());
    PageEmbeddedInWebviewTester tester(
        QUrl::fromLocalFile(absolutePathToHtmlTestFile));

    connect(&tester, &PageEmbeddedInWebviewTester::testResultsCollected,
            [&](QString message) {
                if (message.contains("not ok")) {
                    QFAIL(qPrintable("\n" + message));
                }
            });

    QSignalSpy spy(&tester, SIGNAL(testResultsCollected(QString)));
    QVERIFY(spy.wait());

    TEST_EXCEPTIONS_CATCH
#else
    QSKIP("only works with QWebEngine");
#endif
}

QTEST_MAIN(JavaScriptTests)
