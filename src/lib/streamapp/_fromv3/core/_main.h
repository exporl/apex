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

/**
  *      -- v3tov2 plugin --
  * Do not modify this file by hand,
  * your changes will be overwritten.
  * <^>
  * [ prog ] apex
  * [ file ] core/_main.h
  * [ fmod ] header - main
  * <^>
  */

/**
  * @file main.h
  * The header to be included by all files.
  * Defines the config and includes important headers,
  * the order in which this happens must never be changed.
  */

#ifndef __STR__MAIN_H__
#define __STR__MAIN_H__

#include "../streamapp_conf.h"
#include "../streamapp_namespace.h"
#include "../streamapp_specs.h"

#include "misc/staticassert.h"

#include "platformsetup.h"
#include "types.h"

#define s_track_mem_leaks(x)
#define s_no_throw throw()

/*

  //get version and library name
#include "streamapp_version.h"

  //include macros that should be available everywhere
#include "core/misc/assert.h"
#include "core/misc/macros.h"
#include "core/misc/staticassert.h"
#include "core/misc/throw.h"

  //include type names
#include "core/types.h"

  //include the common OS headers
#include <climits>
#include <cstdarg>
#include <cstdlib>
#include <stdexcept>
#include <typeinfo>
#ifndef S_C6X
  #include <cmath>
  #include <cwchar>
#else
  #include <cstdio>
  #include <string.h>
#endif
#ifdef S_MAC
  #include <cstring>
#endif

  //include the new and delete operator definitions
#include "core/memory/new.h"

  //include the basic headers needed by most classes
  //the macro is there so that the basic headers itself can just include
_main.h,
  //instead of having to include config, platform setup and OS headers
themselves
#ifndef S_NOBASIC_INCLUDE

  #include "core/memory/memory.h"       //links in the allocation methods
  #include "core/misc/tracermacros.h"   //makes sure the printing macros can be
used
  #include "core/utils/exceptions.h"    //includes common exceptions used

#endif

  //platform specific includes
#ifdef _MSC_VER
  #include "win32_msvc.h"
#endif

*/
#endif //#ifndef __STR__MAIN_H__
