/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
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

#ifndef _SYL_SRC_LIB_GLOBAL_H_
#define _SYL_SRC_LIB_GLOBAL_H_

/** @file
 * Global declarations.
 */

#include <QtGlobal>

/** @namespace syl
 * MWF filtering classes.
 */

/** GCC version (e.g., 413).
 * @hideinitializer
 */
#define SYL_GCC_VERSION (__GNUC__ * 100                                     \
                       + __GNUC_MINOR__ * 10                                \
                       + __GNUC_PATCHLEVEL)

/** @def SYL_EXPORT_DECL
 * Exports the following symbol from a DLL.
 */

/** @def SYL_IMPORT_DECL
 * Imports the following symbol from a DLL.
 */

/** @def SYL_EXPORT
 * Exports or imports the following symbol from the library DLL as needed.
 */

/** @def SYL_DEPRECATED
 * Marks the following symbol as deprecated.
 */

#if defined (Q_CC_MSVC) || defined (Q_OS_WIN32)
    #define SYL_EXPORT_DECL __declspec (dllexport)
    #define SYL_IMPORT_DECL __declspec (dllimport)
#else
    // Test for GCC >= 4.0.0
    #if SYL_GCC_VERSION >= 400
        #define SYL_EXPORT_DECL __attribute__ ((visibility ("default")))
        #define SYL_IMPORT_DECL
    #else
        #define SYL_EXPORT_DECL
        #define SYL_IMPORT_DECL
    #endif
#endif

#if defined (Q_CC_MSVC)
    #define SYL_DEPRECATED __declspec (deprecated)
#else
    #define SYL_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifdef SYL_MAKEDLL
    #define SYL_EXPORT SYL_EXPORT_DECL
#else
    #define SYL_EXPORT SYL_IMPORT_DECL
#endif

#endif

