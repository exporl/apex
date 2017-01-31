/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 3.                                               *
 *                                                                            *
 * APEX 3 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 3 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 3.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "fileprefix.h"

#include <QDebug>

using apex::data::FilePrefix;

FilePrefix::FilePrefix() : mType(PREFIX_INLINE)
{
}

bool FilePrefix::operator==(const FilePrefix& other) const
{
    return value() == other.value() && type() == other.type();
}

QString apex::data::FilePrefix::value() const
{
    return mValue;
}

FilePrefix::Type apex::data::FilePrefix::type() const
{
    return mType;
}

void apex::data::FilePrefix::setValue(const QString& v)
{
    mValue = v;
}

void apex::data::FilePrefix::setType(Type t)
{
    Q_ASSERT(t == PREFIX_INLINE || t == PREFIX_MAINCONFIG);
    mType = t;
}
