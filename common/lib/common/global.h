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

#ifndef _EXPORL_COMMON_LIB_COMMON_GLOBAL_H_
#define _EXPORL_COMMON_LIB_COMMON_GLOBAL_H_

#include <QLoggingCategory>

#include <QtGlobal>

#ifdef COMMON_MAKEDLL
#define COMMON_EXPORT Q_DECL_EXPORT
#else
#define COMMON_EXPORT Q_DECL_IMPORT
#endif

#define QL1S(x) QLatin1String(x)
#define QL1C(x) QLatin1Char(x)
#define QSL(str) QStringLiteral(str)

#define DECLARE_PRIVATE(Class)                                                 \
    inline Class##Private *dataFunc()                                          \
    {                                                                          \
        return reinterpret_cast<Class##Private *>(dataPtr);                    \
    }                                                                          \
    inline const Class##Private *dataFunc() const                              \
    {                                                                          \
        return reinterpret_cast<const Class##Private *>(dataPtr);              \
    }                                                                          \
    friend class Class##Private;

#define DECLARE_PRIVATE_SHARED(Class)                                          \
    inline Class##Private *dataFunc()                                          \
    {                                                                          \
        return reinterpret_cast<Class##Private *>(dataPtr.get());              \
    }                                                                          \
    inline const Class##Private *dataFunc() const                              \
    {                                                                          \
        return reinterpret_cast<const Class##Private *>(dataPtr.get());        \
    }                                                                          \
    friend class Class##Private;

#define DECLARE_PUBLIC(Class)                                                  \
    inline Class *pubFunc()                                                    \
    {                                                                          \
        return reinterpret_cast<Class *>(pubPtr);                              \
    }                                                                          \
    inline const Class *pubFunc() const                                        \
    {                                                                          \
        return reinterpret_cast<const Class *>(pubPtr);                        \
    }                                                                          \
    friend class Class;

#define DECLARE_PRIVATE_DATA(Class) Class##Private *const dataPtr;

#define DECLARE_PRIVATE_DATA_SHARED(Class)                                     \
    boost::shared_ptr<Class##Private> dataPtr;

#define DECLARE_PUBLIC_DATA(Class) Class *pubPtr;

#define E_D(Class) Class##Private *const d = dataFunc()
#define E_P(Class) Class *const p = pubFunc()

#define RETURN_IF_FAIL(test)                                                   \
    if (!(test)) {                                                             \
        qCCritical(EXPORL_COMMON, "CRITICAL: !(%s)", #test);                   \
        return;                                                                \
    }
#define RETURN_VAL_IF_FAIL(test, val)                                          \
    if (!(test)) {                                                             \
        qCCritical(EXPORL_COMMON, "CRITICAL: !(%s)", #test);                   \
        return val;                                                            \
    }
#define WARN_DEPRECATED(message)                                               \
    qCWarning(EXPORL_COMMON, "DEPRECATED: %s", message);

// Q_DECLARE_LOGGING_CATEGORY
extern COMMON_EXPORT const QLoggingCategory &EXPORL_COMMON();

// TODO: missing defines in Qt < 5.3, remove after switch to 16.04
#if QT_VERSION < 0x050300
#undef qCDebug
#define qCDebug(category, ...)                                                 \
    for (bool qt_category_enabled = category().isDebugEnabled();               \
         qt_category_enabled; qt_category_enabled = false)                     \
    QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, category().categoryName()) \
        .debug(__VA_ARGS__)
#undef qCWarning
#define qCWarning(category, ...)                                               \
    for (bool qt_category_enabled = category().isWarningEnabled();             \
         qt_category_enabled; qt_category_enabled = false)                     \
    QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, category().categoryName()) \
        .warning(__VA_ARGS__)
#undef qCCritical
#define qCCritical(category, ...)                                              \
    for (bool qt_category_enabled = category().isCriticalEnabled();            \
         qt_category_enabled; qt_category_enabled = false)                     \
    QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, category().categoryName()) \
        .critical(__VA_ARGS__)
#endif
// TODO: missing info messages in Qt < 5.5, remove after switch to 16.04
#if QT_VERSION < 0x050500
#define qCInfo qCDebug
#endif

#endif
