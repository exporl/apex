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

#ifndef _APEX_SRC_LIB_GLOBAL_H_
#define _APEX_SRC_LIB_GLOBAL_H_

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

#ifdef APEX_MAKEDLL
    #define APEX_EXPORT APEX_EXPORT_DECL
#else
    #define APEX_EXPORT APEX_IMPORT_DECL
#endif

#ifdef APEXWRITERS_MAKEDLL
#define APEXWRITERS_EXPORT APEX_EXPORT_DECL
#else
#define APEXWRITERS_EXPORT APEX_IMPORT_DECL
#endif

#ifdef APEXDATA_MAKEDLL
#define APEXDATA_EXPORT APEX_EXPORT_DECL
#else
#define APEXDATA_EXPORT APEX_IMPORT_DECL
#endif

#ifdef APEXPARSERS_MAKEDLL
#define APEXPARSERS_EXPORT APEX_EXPORT_DECL
#else
#define APEXPARSERS_EXPORT APEX_IMPORT_DECL
#endif

#ifdef APEXTOOLS_MAKEDLL
#define APEXTOOLS_EXPORT APEX_EXPORT_DECL
#else
#define APEXTOOLS_EXPORT APEX_IMPORT_DECL
#endif

#ifdef PSIGNIFIT_MAKEDLL
#define PSIGNIFIT_EXPORT APEX_EXPORT_DECL
#else
#define PSIGNIFIT_EXPORT APEX_IMPORT_DECL
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

/**
 * @def FULL_TEMPLATE_EXPORT_INSTANTIATION
 *
 * From Qt's global.h:
 * Compilers which follow outdated template instantiation rules
 * require a class to have a comparison operator to exist when
 * a QList of this type is instantiated. It's not actually
 * used in the list, though. Hence the dummy implementation.
 * Just in case other code relies on it we better trigger a warning
 * mandating a real implementation.
 *
 * Qt doesn't seem to mention it but a dummy qHash function for these
 * classes is also needed.
 *
 * In Apex we need this for classes which are exported in a shared
 * lib because some compilers need a full instantiated class then.
 *
 * @sa DUMMY_COMPARISON_OPERATOR
 * @sa DUMMY_QHASH_FUNCTION
 */
#ifdef Q_CC_MSVC
    #define FULL_TEMPLATE_EXPORT_INSTANTIATION
#endif

/**
 * @def DUMMY_QHASH_FUNCTION
 *
 * The DUMMY_QHASH_FUNCTION defines a simple hash-function for classes.
 *
 * @sa FULL_TEMPLATE_EXPORT_INSTANTIATION
 * @sa DUMMY_COMPARISON_OPERATOR
 */

/**
 * @def DUMMY_COMPARISON_OPERATOR
 *
 * The DUMMY_COMPARISON_OPERATOR defines a simple compare operator for classes.
 *
 * @sa FULL_TEMPLATE_EXPORT_INSTANTIATION
 * @sa DUMMY_QHASH_FUNCTION
 */

//example:
//class APEXDATA_EXPORT Foo : public QList<Bar> {...}; -> error
//add DUMMY_QHASH_FUNCTION(Bar) to the global namespace
//and DUMMY_COMPARISON_OPERATOR(Bar) to Bar's definition
#ifdef FULL_TEMPLATE_EXPORT_INSTANTIATION
    #define DUMMY_QHASH_FUNCTION(C)                                     \
        inline uint qHash(const C)                                                   \
        {                                                               \
            qFatal("Dummy inline uint qHash(const "#C") was called");   \
            return 0;                                                   \
        }

    #define DUMMY_COMPARISON_OPERATOR(C)                                \
        bool operator==(const C&) const                                 \
        {                                                               \
            qFatal("Dummy "#C"::operator==(const "#C"&) was called");   \
            return false;                                               \
        }
#else
    #define DUMMY_QHASH_FUNCTION(C)
    #define DUMMY_COMPARISON_OPERATOR(C)
#endif

#endif

