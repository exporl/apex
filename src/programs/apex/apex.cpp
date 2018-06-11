/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "apexmain/apexcontrol.h"

#include "apexmain/errorhandler.h"

#include "apextools/global.h"

#include "common/debug.h"

#include <QApplication>
#include <QMessageBox>

#ifdef WIN32
#include <windows.h>
#endif

#if defined(Q_OS_ANDROID)
#include <QtWebView/QtWebView>
#endif

using namespace apex;
using namespace cmn;

static void coreDumpCallback(const QString &path)
{
    QMessageBox::critical(
        nullptr, QSL("Apex crashed"),
        QString::fromLatin1("Apex has crashed, please send %1 to the Apex "
                            "developers at exporladmin@ls.kuleuven.be. Along "
                            "with the contents of the details section found in "
                            "help->about on the Apex main screen.")
            .arg(path));
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
#if defined(Q_OS_ANDROID)
    QtWebView::initialize();
#endif
    app.setApplicationName(applicationName);
    app.setOrganizationName(organizationName);
    app.setOrganizationDomain(organizationDomain);

    enableCoreDumps(QCoreApplication::applicationFilePath(), coreDumpCallback);

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
    ApexControl apexControl;
    return QCoreApplication::exec();
}
