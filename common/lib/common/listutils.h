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

#ifndef _EXPORL_COMMON_LIB_COMMON_LISTUTILS_H_
#define _EXPORL_COMMON_LIB_COMMON_LISTUTILS_H_

#include "global.h"

#include <QList>
#include <QMap>
#include <QMetaType>
#include <QPair>
#include <QString>
#include <QVariant>

namespace cmn
{

// We need the typedefs (besides readability), otherwise Q_FOREACH does not work
// with the value_type
typedef QPair<QString, QString> StringStringPair;
typedef QList<StringStringPair> StringStringList;
typedef QMap<QString, QString> StringStringMap;
typedef QPair<QString, QVariant> StringVariantPair;
typedef QList<StringVariantPair> StringVariantList;
typedef QMap<QByteArray, QByteArray> ArrayArrayMap;
typedef QList<QPair<QString, StringStringMap>> StringMapList;

template <typename Container>
inline typename Container::const_iterator
checkIter(const Container &container, typename Container::const_iterator iter)
{
    Q_ASSERT(iter != container.constEnd());
    return iter;
}

// Search a list
template <typename Container, typename T>
inline typename Container::value_type checkedFind(const Container &container,
                                                  const T &value)
{
    typename Container::const_iterator iter = qFind(container, value);
    Q_ASSERT(iter != container.constEnd());
    return *iter;
}

// Search a list of pairs
template <typename Container, typename T>
inline typename Container::value_type::second_type
checkedPairFind(const Container &container, const T &value)
{
    typename Container::const_iterator first = container.constBegin();
    typename Container::const_iterator last = container.constEnd();
    while (first != last && !(first->first == value))
        ++first;
    Q_ASSERT_X(first != last, "checkedPairFind", "value not found");
    return first->second;
}

// Search a list of pairs, return a default value if not found
template <typename Container, typename Key, typename Value>
inline typename Container::value_type::second_type
checkedPairFind(const Container &container, const Key &key,
                const Value &defaultValue)
{
    typename Container::const_iterator first = container.constBegin();
    typename Container::const_iterator last = container.constEnd();
    while (first != last && !(first->first == key))
        ++first;
    if (first == last)
        return defaultValue;
    return first->second;
}

// we need operator<< inline(otherwise it is not working with anonymous QMaps)
// Use where you need a QMap<..., ...> with
//   MapMaker<..., ...>() << qMakePair(..., ...) << ...
template <typename Key, typename Value>
class MapMaker
{
public:
    MapMaker<Key, Value> &operator<<(const QPair<Key, Value> &value)
    {
        map.insert(value.first, value.second);
        return *this;
    }

    operator QMap<Key, Value> &()
    {
        return map;
    }

private:
    QMap<Key, Value> map;
};

} // namespace cmn

Q_DECLARE_METATYPE(cmn::StringVariantList)
Q_DECLARE_METATYPE(cmn::StringStringMap)
Q_DECLARE_METATYPE(cmn::StringStringList)

#endif
