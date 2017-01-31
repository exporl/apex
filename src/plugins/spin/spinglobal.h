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

#ifndef _SPIN_LIB_GLOBAL_H_
#define _SPIN_LIB_GLOBAL_H_

#include <QtGlobal>

#define GCC_VERSION (__GNUC__ * 100                                         \
                   + __GNUC_MINOR__ * 10                                    \
                   + __GNUC_PATCHLEVEL)

#if defined (Q_CC_MSVC) || defined (Q_OS_WIN32)
    #define APEX_EXPORT_DECL __declspec (dllexport)
    #define APEX_IMPORT_DECL __declspec (dllimport)
#else
    // Test for GCC >= 4.0.0
    #if GCC_VERSION >= 400
        #define APEX_EXPORT_DECL __attribute__ ((visibility ("default")))
        #define APEX_IMPORT_DECL
    #else
        #define APEX_EXPORT_DECL
        #define APEX_IMPORT_DECL
    #endif
#endif

#if defined (Q_CC_MSVC)
    #define DECLARE_DEPRECATED __declspec (deprecated)
#else
    #define DECLARE_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifdef SPIN_MAKEDLL
    #define SPIN_EXPORT APEX_EXPORT_DECL
#else
    #define SPIN_EXPORT APEX_IMPORT_DECL
#endif


#define DECLARE_PRIVATE(Class)                                              \
        inline Class##Private* dataFunc()                                   \
        {                                                                   \
            return reinterpret_cast<Class##Private *> (dataPtr.get());      \
        }                                                                   \
        inline const Class##Private* dataFunc() const                       \
        {                                                                   \
            return reinterpret_cast<const Class##Private *> (dataPtr.get());\
        }                                                                   \
        friend class Class##Private;

#define DECLARE_PUBLIC(Class)                                               \
        inline Class* pubFunc()                                             \
        {                                                                   \
            return reinterpret_cast<Class *> (pubPtr);                      \
        }                                                                   \
        inline const Class* pubFunc() const                                 \
        {                                                                   \
            return reinterpret_cast<const Class *> (pubPtr);                \
        }                                                                   \
        friend class Class;

#define DECLARE_PRIVATE_DATA(Class)                                         \
        const boost::scoped_ptr<Class##Private> dataPtr;

#define DECLARE_PRIVATE_DATA_SHARED(Class)                                  \
        boost::shared_ptr<Class##Private> dataPtr;

#define DECLARE_PUBLIC_DATA(Class)                                          \
        Class * pubPtr;

#define D(Class) Class##Private * const d = dataFunc()
#define P(Class) Class * const p = pubFunc()

#endif

