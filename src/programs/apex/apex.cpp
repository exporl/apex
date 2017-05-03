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

#include "apexmain/apexcontrol.h"

#include "apexmain/errorhandler.h"

#include "apextools/global.h"

#include "common/debug.h"

#include <QApplication>

#ifdef WIN32
#include <windows.h>
#endif

using namespace apex;
using namespace cmn;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(applicationName);
    app.setOrganizationName(organizationName);
    app.setOrganizationDomain(organizationDomain);

    enableCoreDumps(QCoreApplication::applicationFilePath());

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

    ErrorHandler handler;
    return ApexControl().exec();
}
