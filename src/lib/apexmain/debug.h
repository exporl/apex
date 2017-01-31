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

#ifndef _RBA_SRC_LIB_PLUMBING_DEBUG_H_
#define _RBA_SRC_LIB_PLUMBING_DEBUG_H_

#include "global.h"

#include <QString>

// Generate core dumps for unexpected errors in Windows; does not work with
// assert/abort. Additionally, enables report on memory leaks and coredumps with
// Q_ASSERT
APEX_EXPORT void enableCoreDumps(const QString& applicationFilePath);

#endif

