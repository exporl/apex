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

#include "buttongroup.h"

#include <algorithm>

namespace apex
{
namespace data
{

ButtonGroup::const_iterator::const_iterator(const QList<QString>::const_iterator& it)
{
    *this = it;
}

ButtonGroup::const_iterator& ButtonGroup::const_iterator::
        operator=(const QList<QString>::const_iterator& it)
{
    this->it = it;
    return *this;
}
ButtonGroup::const_iterator& ButtonGroup::const_iterator::operator++() //prefix++
{
    it++;
    return *this;
}

ButtonGroup::const_iterator ButtonGroup::const_iterator::operator++(int) //postfix++
{
    const_iterator copy(it);
    ++(*this);
    return copy;
}

bool ButtonGroup::const_iterator::operator==(const const_iterator& other) const
{
    return it == other.it;
}

bool ButtonGroup::const_iterator::operator!=(const const_iterator& other) const
{
    return !operator==(other);
}

const QString& ButtonGroup::const_iterator::operator*() const
{
    return *it;
}

}
}

QList<QString>::const_iterator apex::data::ButtonGroup::begin() const
{
    return buttonIds.begin();
}

QList<QString>::const_iterator apex::data::ButtonGroup::end() const
{
    return buttonIds.end();
}

int apex::data::ButtonGroup::size() const
{
    return buttonIds.size();
}

void apex::data::ButtonGroup::append(const QString& id)
{
    buttonIds.append(id);
}

apex::data::ButtonGroup::ButtonGroup( const QString& ac_sID )
    : id( ac_sID )
{
}

bool apex::data::ButtonGroup::IsElement( const QString& ac_sID ) const
{
        const_iterator it = std::find( begin(), end(), ac_sID );
    return it != end();
}

bool apex::data::ButtonGroup::operator==(const apex::data::ButtonGroup& other) const
{
    if (size() != other.size())
        return false;

    //check if all element of this are also element of other
    for (const_iterator it = begin(); it != end(); it++)
    {
        if (!other.IsElement(*it))
            return false;
    }

    //check if all elements of other are also elements of this
    for (const_iterator it = other.begin(); it != other.end(); it++)
    {
        if (!IsElement(*it))
            return false;
    }

    return id == other.id;
}

bool apex::data::ButtonGroup::operator!=(const apex::data::ButtonGroup& other) const
{
    return !(*this == other);
}
