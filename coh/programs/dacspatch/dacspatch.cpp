/******************************************************************************
 * Copyright (C) 2008  Michael Hofmann <mh21@piware.de>                       *
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

// patches NIC to accept high-rate 5cpc frames with electrodes 0 and 31

#include "coh/coh.h"

#include <QFile>

int main(int argc, char *argv[])
{
    if (argc < 2)
        return 1;

    QFile original(QFile::decodeName(argv[1]));
    if (!original.open(QIODevice::ReadOnly))
        return 2;
    QByteArray contents = original.readAll();
    int cmp1 = -1, cmp2;
    Q_FOREVER {
        cmp1 = contents.indexOf("\x83\xf8\x01\x7c", cmp1 + 1);
        if (cmp1 == -1) {
            qFatal("Unable to find cmp1");
            return 3;
        }
        qCDebug(EXPORL_COH, "cmp eax, 1; jl at %d", cmp1);
        cmp2 = contents.indexOf("\x83\xf8\x1e", cmp1 + 5);
        if (cmp2 == -1) {
            qFatal("Unable to find cmp2");
            return 3;
        }
        qCDebug(EXPORL_COH, "cmp eax, 30; jle at %d", cmp2);
        if (cmp2 == cmp1 + 5) {
            break;
        }
    }

    qCDebug(EXPORL_COH, "Found range check at %d", cmp1);
    qCDebug(EXPORL_COH, "Before: %s", contents.mid(cmp1, 10).toHex().data());
    contents[cmp1 + 2] = 0;
    contents[cmp2 + 2] = 31;
    qCDebug(EXPORL_COH, "After : %s", contents.mid(cmp1, 10).toHex().data());

    QFile patched(QFile::decodeName(argv[1]) + QLatin1String(".patched"));
    if (!patched.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return 2;
    patched.write(contents);
    patched.close();

    return 0;
}
