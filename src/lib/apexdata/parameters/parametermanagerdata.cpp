/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "parametermanagerdata.h"

#include "parameter.h"

#include <QDebug>

namespace apex
{

namespace data
{

void ParameterManagerData::registerParameter(const QString &id,
                                             const Parameter &pn)
{
    // qCDebug(APEX_RS) << "registering parameter" << id;
    if (!id.isEmpty())
        idMap[id] = pn;
}

Parameter ParameterManagerData::parameterById(const QString &id) const
{
    return idMap[id];
}

void ParameterManagerData::showContents() const
{

    qCDebug(APEX_RS) << "Registered parameters:";
    qCDebug(APEX_RS) << "======================";

    IdParameterMap::const_iterator it;
    for (it = idMap.begin(); it != idMap.end(); ++it) {
        qCDebug(APEX_RS).nospace() << "id=" << it.key()
                                   << ", name=" << it.value().toString();
    }
}

bool ParameterManagerData::operator==(const ParameterManagerData &other) const
{
    return idMap == other.idMap;
}

} // ns data
} // ns apex
