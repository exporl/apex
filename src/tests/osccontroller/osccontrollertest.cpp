#include "../lib/apexmain/device/plugincontrollerinterface.h"

#include "common/debug.h"
#include "common/global.h"

#include "osccontrollertest.h"

#include <QDebug>
#include <QPluginLoader>

using namespace apex;
// using namespace device;

void OscControllerTest::initTestCase()
{
    cmn::enableCoreDumps(QCoreApplication::applicationFilePath());
}

QString libraryName(const QString &base)
{
    QString path(QCoreApplication::applicationDirPath());
#ifdef Q_OS_WIN32
    return path + QLatin1String("\\") + base + QLatin1String(".dll");
#else
    return path + QLatin1String("/") + QLatin1String("lib") + base +
           QLatin1String(".so");
#endif
}

void OscControllerTest::testPluginLoader()
{
#ifdef Q_OS_WIN32
    QPluginLoader loader(libraryName(QSL("osccontroller")));
    QObject *plugin = loader.instance();

    QVERIFY(loader.isLoaded());
    QVERIFY(plugin);

    PluginControllerCreator *creator =
        qobject_cast<PluginControllerCreator *>(plugin);

    QVERIFY(creator);

#endif
}

// generate standalone binary for the test
QTEST_MAIN(OscControllerTest)
