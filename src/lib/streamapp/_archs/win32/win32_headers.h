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

#ifndef __STR_WIN32_HEADERS_H__
#define __STR_WIN32_HEADERS_H__

#include "../../_fromv3/core/_main.h"

#include <Objbase.h>
#include <WinSock2.h>
#include <windows.h>

#ifdef _MSC_VER
#pragma warning(disable : 4201)
#endif

#include <mmsystem.h>
#include <winioctl.h>

#ifdef _MSC_VER
#pragma warning(default : 4201)
#endif

// use default alignment for speed
#ifdef PACKED
#undef PACKED
#endif

#endif //#ifndef __STR_WIN32_HEADERS_H__
