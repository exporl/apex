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

#ifndef _APEX_SRC_LIB_APEXTOOLS_GLOBAL_H_
#define _APEX_SRC_LIB_APEXTOOLS_GLOBAL_H_

#include "common/global.h"

#include <QLoggingCategory>
#include <QString>

#include <QtGlobal>

namespace apex
{
const QString applicationName = QSL("APEX");
const QString organizationName = QSL("ExpORL");
const QString organizationDomain = QSL("exporl.med.kuleuven.be");
}

#ifdef APEX_MAKEDLL
#define APEX_EXPORT Q_DECL_EXPORT
#else
#define APEX_EXPORT Q_DECL_IMPORT
#endif

#ifdef APEXWRITERS_MAKEDLL
#define APEXWRITERS_EXPORT Q_DECL_EXPORT
#else
#define APEXWRITERS_EXPORT Q_DECL_IMPORT
#endif

#ifdef APEXDATA_MAKEDLL
#define APEXDATA_EXPORT Q_DECL_EXPORT
#else
#define APEXDATA_EXPORT Q_DECL_IMPORT
#endif

#ifdef APEXPARSERS_MAKEDLL
#define APEXPARSERS_EXPORT Q_DECL_EXPORT
#else
#define APEXPARSERS_EXPORT Q_DECL_IMPORT
#endif

#ifdef APEXTOOLS_MAKEDLL
#define APEXTOOLS_EXPORT Q_DECL_EXPORT
#else
#define APEXTOOLS_EXPORT Q_DECL_IMPORT
#endif

#ifdef APEXSPIN_MAKEDLL
#define APEXSPIN_EXPORT Q_DECL_EXPORT
#else
#define APEXSPIN_EXPORT Q_DECL_IMPORT
#endif

#ifdef PSIGNIFIT_MAKEDLL
#define PSIGNIFIT_EXPORT Q_DECL_EXPORT
#else
#define PSIGNIFIT_EXPORT Q_DECL_IMPORT
#endif

#ifdef CALIBRATIONADMIN_MAKEDLL
#define CALIBRATIONADMIN_EXPORT Q_DECL_EXPORT
#else
#define CALIBRATIONADMIN_EXPORT Q_DECL_IMPORT
#endif

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

// example:
// class APEXDATA_EXPORT Foo : public QList<Bar> {...}; -> error
// add DUMMY_QHASH_FUNCTION(Bar) to the global namespace
// and DUMMY_COMPARISON_OPERATOR(Bar) to Bar's definition
#ifdef FULL_TEMPLATE_EXPORT_INSTANTIATION
#define DUMMY_QHASH_FUNCTION(C)                                                \
    inline uint qHash(const C)                                                 \
    {                                                                          \
        qFatal("Dummy inline uint qHash(const " #C ") was called");            \
        return 0;                                                              \
    }

#define DUMMY_COMPARISON_OPERATOR(C)                                           \
    bool operator==(const C &) const                                           \
    {                                                                          \
        qFatal("Dummy " #C "::operator==(const " #C "&) was called");          \
        return false;                                                          \
    }
#else
#define DUMMY_QHASH_FUNCTION(C)
#define DUMMY_COMPARISON_OPERATOR(C)
#endif

// Q_DECLARE_LOGGING_CATEGORY
extern APEXTOOLS_EXPORT const QLoggingCategory &APEX_RS();
extern APEXTOOLS_EXPORT const QLoggingCategory &APEX_THREADS();
#endif
