/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
 * Portions Copyright (C) 1992-2007 Trolltech ASA.                            *
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

#include "utils.h"

#include <QMetaProperty>

#ifdef Q_OS_WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace cmn
{

const char *enumValueToKey(const QObject *object, const char *property)
{
    const QMetaObject *const meta = object->metaObject();
    const int propertyIndex = meta->indexOfProperty(property);
    Q_ASSERT(propertyIndex != -1);
    const QMetaProperty metaProperty = meta->property(propertyIndex);
    Q_ASSERT_X(metaProperty.isEnumType(), "enumValueToKey",
               "Not an enum type: " + QByteArray(metaProperty.typeName()));
    QMetaEnum metaEnum = metaProperty.enumerator();
    return metaEnum.valueToKey(object->property(property).toInt());
}

const char *enumValueToKey(const QMetaObject &metaObject, const char *type,
                           int value)
{
    const int metaEnumIndex = metaObject.indexOfEnumerator(type);
    Q_ASSERT(metaEnumIndex >= 0);
    const QMetaEnum metaEnum = metaObject.enumerator(metaEnumIndex);
    const char *const key = metaEnum.valueToKey(value);
    Q_ASSERT(key);
    return key;
}

QByteArray enumValueToKeys(const QMetaObject &metaObject, const char *type,
                           int value)
{
    const int metaEnumIndex = metaObject.indexOfEnumerator(type);
    Q_ASSERT(metaEnumIndex >= 0);
    const QMetaEnum metaEnum = metaObject.enumerator(metaEnumIndex);
    return metaEnum.valueToKeys(value);
}

int enumKeyToValue(const QMetaObject &metaObject, const char *type,
                   const char *key)
{
    const int metaEnumIndex = metaObject.indexOfEnumerator(type);
    Q_ASSERT(metaEnumIndex >= 0);
    const QMetaEnum metaEnum = metaObject.enumerator(metaEnumIndex);
    const int enumValue = metaEnum.keyToValue(key);
    Q_ASSERT(enumValue >= 0);
    return enumValue;
}

QByteArray stdStringToByteArray(const std::string &string)
{
    return QByteArray(string.data(), string.size());
}

QByteArray stdStringToRawByteArray(const std::string &string)
{
    return QByteArray::fromRawData(string.data(), string.size());
}

void milliSleep(unsigned millis)
{
#ifdef Q_OS_WIN32
    Sleep(millis);
#else
    usleep(1000 * millis);
#endif
}

// copied and modified from qobject.cpp
void connectSlotsByNameToPrivate(QObject *publicObject, QObject *privateObject)
{
    if (!publicObject)
        return;
    const QMetaObject *mo = privateObject->metaObject();
    Q_ASSERT(mo);
    const QObjectList list =
#if QT_VERSION < 0x050000
        qFindChildren<QObject *>(publicObject, QString());
#else
        publicObject->findChildren<QObject *>(QString());
#endif
    for (int i = 0; i < mo->methodCount(); ++i) {
#if QT_VERSION < 0x050000
        const char *slot = mo->method(i).signature();
#else
        const QByteArray slotSignature = mo->method(i).methodSignature();
        const char *slot = slotSignature.constData();
#endif
        Q_ASSERT(slot);
        if (slot[0] != 'o' || slot[1] != 'n' || slot[2] != '_')
            continue;
        bool foundIt = false;
        for (int j = 0; j < list.count(); ++j) {
            const QObject *co = list.at(j);
            const QByteArray coName = co->objectName().toLatin1();
            if (coName.isEmpty() ||
                qstrncmp(slot + 3, coName.constData(), coName.size()) ||
                slot[coName.size() + 3] != '_')
                continue;
            const char *signal = slot + coName.length() + 4;
            const QMetaObject *smo = co->metaObject();
            int sigIndex = smo->indexOfMethod(signal);
            if (sigIndex < 0) { // search for compatible signals
                int slotlen = qstrlen(signal) - 1;
                for (int k = 0; k < co->metaObject()->methodCount(); ++k) {
                    if (smo->method(k).methodType() != QMetaMethod::Signal)
                        continue;

                    if (!qstrncmp(
#if QT_VERSION < 0x050000
                            smo->method(k).signature(),
#else
                            smo->method(k).methodSignature().constData(),
#endif
                            signal, slotlen)) {
                        sigIndex = k;
                        break;
                    }
                }
            }
            if (sigIndex < 0)
                continue;
            if (QMetaObject::connect(co, sigIndex, privateObject, i)) {
                foundIt = true;
                break;
            }
        }
        if (foundIt) {
            // we found our slot, now skip all overloads
            while (mo->method(i + 1).attributes() & QMetaMethod::Cloned)
                ++i;
        } else if (!(mo->method(i).attributes() & QMetaMethod::Cloned)) {
            qCWarning(EXPORL_COMMON,
                      "connectSlotsByName: No matching signal for %s", slot);
        }
    }
}

} // namespace cmn
