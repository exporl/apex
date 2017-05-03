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

#include "systemutils.h"

#include <QString>
#include <QFile>

using namespace std;

namespace
{
const int sc_nBufferSize = 4096;
}

bool systemutils::f_bCopyFile(const QString& ac_sInputFile, const QString &ac_sOutputFile)
{
    //try open files
    QFile inFile(ac_sInputFile);

    if (!inFile.open(QIODevice::ReadOnly))
        return false;

    QFile outFile(ac_sOutputFile);

    if (!outFile.open(QIODevice::WriteOnly))
        return false;

    //copy
    char buff[ sc_nBufferSize ];
    qint64 readBytes = sc_nBufferSize;

    while (readBytes == sc_nBufferSize) {
        qint64 readBytes = inFile.read(buff, sc_nBufferSize);
        outFile.write(buff, readBytes);
    }
    inFile.close();
    outFile.close();
    return true;
}

bool systemutils::f_bFileExists(const QString &ac_sFile)
{
    return QFile::exists(ac_sFile);
}

bool systemutils::f_bClearFile(const QString &ac_sFile)
{
    QFile inFile(ac_sFile);
    if (!inFile.open(QIODevice::Truncate))
        return false;
    inFile.close();
    return true;
}
