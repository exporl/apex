/******************************************************************************
* Copyright (C) 2009  Job Noorman <jobnoorman@gmail.com>                     *
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

#include "consolestatusreporter.h"

#include <QStringList>

#include <iomanip>

#define COLOR_ERROR   "\033[31m"
#define COLOR_WARNING "\033[33m"
#define COLOR_MESSAGE "\033[32m"
#define COLOR_RESET   "\033[0m"


namespace apex
{

void ConsoleStatusReporter::report(const StatusItem& error)
{
/*    std::ostream& stream = error.level() >= StatusItem::ERROR ? std::cerr :
                                                                std::cout;

    switch (error.level())
    {
        case StatusItem::ERROR:
            stream << COLOR_ERROR;
            break;
        case StatusItem::WARNING:
            stream << COLOR_WARNING;
            break;
        case StatusItem::MESSAGE:
            stream << COLOR_MESSAGE;
            break;
        default:
            break;
    }*/

    QString src = error.source().isEmpty() ? "APEX" : qPrintable(error.source());
    qCDebug(APEX_RS, "%s:: %s", qPrintable(src), qPrintable(error.message()));

  /*  Q_FOREACH (QString line, msgLines)
    {
        stream << std::endl
               << std::setw(src.size() + line.size() + 1)
               << qPrintable(line);
    }

    stream << COLOR_RESET; //<< std::endl;*/
}

}

