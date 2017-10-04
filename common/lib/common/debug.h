/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
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

/** @file
 * Debug function declaration.
 */

#ifndef _EXPORL_COMMON_LIB_COMMON_DEBUG_H_
#define _EXPORL_COMMON_LIB_COMMON_DEBUG_H_

#include "global.h"

#include <QCoreApplication>
#include <QString>

namespace cmn
{
typedef void (*CoreDumpCallback)(const QString &path);

/** Break into the debugger. For Unix, this raises a SIGINT signal that is
 * catched by gdb.
 */
COMMON_EXPORT void debugger();

// Generate core dumps for unexpected errors in Windows; does not work with
// assert/abort. Additionally, enables report on memory leaks and coredumps with
// Q_ASSERT
COMMON_EXPORT void enableCoreDumps(const QString &applicationFilePath,
                                   CoreDumpCallback callback = nullptr);

COMMON_EXPORT void dumpQObjectInfo(QObject *object);

} // namespace cmn

#endif
