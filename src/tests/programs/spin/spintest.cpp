#include <QPluginLoader>
#include <QApplication>

#include "xml/xercesinclude.h"

#include "runner/pluginrunnerinterface.h"

#include <fstream>


using namespace xercesc;

static void crashOnAssert ( QtMsgType type, const char *msg );

/**
 * To execute this program correctly, the pwd must be set to bin/debug!
 */
int main ( int argc, char *argv[] )
{
    qInstallMsgHandler ( crashOnAssert );
    XMLPlatformUtils::Initialize();
    QApplication app ( argc, argv );

    app.setApplicationName ( "APEX" );
    app.setOrganizationName ( "ExpORL" );
    app.setOrganizationDomain ( "exporl.med.kuleuven.be" );

#ifdef WIN32
    QPluginLoader loader ( "libspin.dll" );
#else
    QPluginLoader loader ( "libspin.so" );
#endif

    loader.load();
    PluginRunnerCreator *creator = qobject_cast<PluginRunnerCreator*>
                                   ( loader.instance() );
    PluginRunnerInterface* spinplugin = creator->createRunner ( "spin", "../../" );
    spinplugin->getFileName();
    delete spinplugin;

    XMLPlatformUtils::Terminate();
    app.exit();
}

static void crashOnAssert ( QtMsgType type, const char *msg )
{
    fprintf ( stderr, "%s\n", msg );
    fflush ( stderr );

    if ( type == QtFatalMsg ||
        (type == QtWarningMsg && ( !qgetenv ( "QT_FATAL_WARNINGS" ).isNull() )))
    {
        * ( reinterpret_cast<int*> ( 0 ) ) = 0;
    }
}
