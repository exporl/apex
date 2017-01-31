/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 3.                                               *
 *                                                                            *
 * APEX 3 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 3 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 3.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "appcore/qt_msghandler.h"

#include "services/application.h"
#include "services/servicemanager.h"

#include "utils/stdredirector.h"

#include "apexcontrol.h"
#include "apexloader.h"
#include "debug.h"

#include <iostream>

#ifdef WIN32
#include <windows.h>
#endif

using namespace std;
using namespace apex;

int ApexLoader::Load( int argc, char *argv[] )
{
#ifdef NOCONSOLE
    appcore::QtMsgHandler::sf_pInstance();
    utils::StdRedirector::sf_pInstance();
#endif

    // make sure apex does not crash if the user tries to close the console window
    // disabled until we can make sure that apex is still closable if not
    // responding/crashing
#if defined(Q_OS_WIN32) && 0
    HWND console = GetConsoleWindow();
    if (console) {
        SetConsoleCtrlHandler(NULL, true);
        HMENU menu = GetSystemMenu(console, false);
        DeleteMenu(menu, SC_CLOSE, MF_BYCOMMAND);
    }
#endif

    ServiceManager& services = ServiceManager::Get();
    services.SetCommandLine (argc, argv);

    // make sure the application is constructed before ApexControl to
    // avoid thread problems with QT thinking ApexControl and Application being
    // in two different threads. This causes all kinds of funny effects
    // because signal-slot connections then stop being simple function calls
    (void) apex::Application::Get();

    enableCoreDumps(QCoreApplication::applicationFilePath());

    int nRetCode = apex::ApexControl::Get().exec();
    // Deletion of the main window by C++ causes segfaults and QThread warnings
    // Disposing the services here helps.
    // stijn:
    //   if we do not delete from within the main thread,
    //   QApplication's shutdown raises a warning:
    //   "QThreadStorage: thread xxx exited after QThreadStorage destroyed"
    services.RemoveServices();

    return nRetCode;
}
