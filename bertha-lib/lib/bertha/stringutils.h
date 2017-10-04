/******************************************************************************
 * Copyright (C) 2008 Michael Hofmann <mh21@piware.de>                        *
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

#ifndef _BERTHA_SRC_LIB_BERTHA_STRINGUTILS_H_
#define _BERTHA_SRC_LIB_BERTHA_STRINGUTILS_H_

#include "global.h"

#include <QMetaEnum>
#include <QMetaProperty>
#include <QStringList>
#include <QVector>

namespace bertha
{

QStringList stringToStringList(const QString value);
QString stringListToString(const QStringList value);

QVector<double> stringToVector(const QString value);
QString vectorToString(const QVector<double> value);

QVector<QVector<double>> stringToDoubleVector(const QString value);
QString doubleVectorToString(const QVector<QVector<double>> value);

int stringToEnumValue(const QString value, const QMetaEnum enumerator);
QString enumValueToKey(const QMetaProperty &property, int value);

} // namespace bertha

#endif
