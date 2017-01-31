#include "apexmain/runner/pluginrunnerinterface.h"

#include "apextools/global.h"

#include "apextools/xml/xercesinclude.h"

#include <QApplication>
#include <QPluginLoader>

#include <fstream>

using namespace xercesc;

/**
 * To execute this program correctly, the pwd must be set to bin/debug!
 */
int main ( int argc, char *argv[] )
{
    XMLPlatformUtils::Initialize();
    QApplication app ( argc, argv );

    app.setApplicationName (apex::applicationName);
    app.setOrganizationName (apex::organizationName);
    app.setOrganizationDomain (apex::organizationDomain);

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
