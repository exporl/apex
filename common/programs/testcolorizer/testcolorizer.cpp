/******************************************************************************
 * Copyright (C) 2013  Michael Hofmann <mh21@mh21.de>                         *
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

#include <QTextStream>

#ifdef Q_OS_UNIX
#include <unistd.h>
#endif

const char *consoleColors[] = {"\033[00m", "\033[02;31m", "\033[02;33m",
                               "\033[02;32m"};
const char *noColors[] = {"", "", "", ""};

int main()
{
    QTextStream in(stdin);
    QTextStream out(stdout);

    const char **colors =
#ifdef Q_OS_UNIX
        isatty(fileno(stdout)) ? consoleColors :
#endif
                               noColors;
    const char *color = colors[0];

    do {
        QString line = in.readLine();
        if (line.startsWith(QLatin1String("FAIL")))
            color = colors[1];
        else if (line.startsWith(QLatin1String("SKIP")))
            color = colors[2];
        else if (line.startsWith(QLatin1String("PASS")))
            color = colors[3];
        else if (line.startsWith(QLatin1String("QDEBUG")))
            color = colors[0];
        else if (line.startsWith(QLatin1String(" ")))
            ; // continuation, do nothing
        else
            color = colors[0];
        out << color << line << colors[0] << "\n";
        out.flush();
    } while (!in.atEnd());
    return 0;
}
