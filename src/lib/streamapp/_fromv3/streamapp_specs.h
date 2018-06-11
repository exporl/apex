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
  * [ file ] streamapp_specs.h
  * [ fmod ] default
  * <^>
  */

/**
  * @file streamapp_specs.h
  * enable or disable specific features depending
  * on platform and macros from streamapp_conf.h.
  */
#ifndef __STR_STREAMAPP_SPECS_H__
#define __STR_STREAMAPP_SPECS_H__

// only use tracking if available, this is the case
// with the custom heap or with MSVC only.
#ifdef S_TRACK_MEMORY
#if !defined(S_WIN32) || defined(S_NO_ALLOCATOR_STUFF) || defined(__GNUC__)
#undef S_TRACK_MEMORY
#endif
#endif

#if defined(S_NO_ALLOCATOR_STUFF) && !defined(S_NO_GLOBAL_OPERATOR_OVERLOAD)
#define S_NO_GLOBAL_OPERATOR_OVERLOAD
#endif

// check if we're on a real pc
#ifndef S_PC
#undef S_USE_QT_GRAPHICS
#endif

// check if freetype can be used
#if defined(S_USE_GRAPHICS) && !defined(S_USE_NO_FREETYPE)
#if defined(S_PC) || defined(S_ARM) || defined(S_PSP)
#define S_USE_FREETYPE
#endif
#endif

#endif //#ifndef __STR_STREAMAPP_SPECS_H__
