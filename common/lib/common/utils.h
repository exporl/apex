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

#ifndef _EXPORL_COMMON_LIB_COMMON_UTILS_H_
#define _EXPORL_COMMON_LIB_COMMON_UTILS_H_

#include "global.h"

#include <QMap>
#include <QMutex>

struct QMetaObject;
class QObject;

namespace cmn
{

/** Retrieves an enum QObject property and returns the key for the current
 * value.
 *
 * @param object QObject for which the property should be returned
 * @param property enum property
 * @return key for the current value of the property
 */
COMMON_EXPORT const char *enumValueToKey(const QObject *object,
                                         const char *property);

COMMON_EXPORT const char *enumValueToKey(const QMetaObject &metaObject,
                                         const char *type, int value);
COMMON_EXPORT QByteArray enumValueToKeys(const QMetaObject &metaObject,
                                         const char *type, int value);
COMMON_EXPORT int enumKeyToValue(const QMetaObject &metaObject,
                                 const char *type, const char *key);

COMMON_EXPORT QByteArray stdStringToByteArray(const std::string &string);
COMMON_EXPORT QByteArray stdStringToRawByteArray(const std::string &string);

COMMON_EXPORT void milliSleep(unsigned millis);

COMMON_EXPORT void connectSlotsByNameToPrivate(QObject *publicObject,
                                               QObject *privateObject);

template <typename T>
class QObjectDatabase
{
public:
    T *get(const QString &name) const
    {
        QMutexLocker locker(&lock);

        return data.value(name);
    }

    bool insert(const QString &name, T *object)
    {
        QMutexLocker locker(&lock);

        if (data.value(name))
            return false;
        data.insert(name, object);
        return true;
    }

    void remove(const QString &name)
    {
        QMutexLocker locker(&lock);

        data.remove(name);
    }

private:
    mutable QMutex lock;
    QMap<QString, T *> data;
};

} // namespace cmn

#endif
