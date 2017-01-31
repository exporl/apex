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

#include "apextools/apextools.h"

#include "screenlayoutelement.h"

#include <algorithm>

namespace apex
{
namespace data
{

ScreenLayoutElement::~ScreenLayoutElement()
{
    Q_ASSERT( getNumberOfChildren() == 0 );
}

ScreenLayoutElement::ScreenLayoutElement( const QString& id, ScreenElement* parent )
    : ScreenElement( id, parent )
{
}

const std::vector<ScreenElement*>* ScreenLayoutElement::getChildren() const
{
    return &children;
}

std::vector<ScreenElement*>* ScreenLayoutElement::getChildren()
{
    return &children;
}

int ScreenLayoutElement::getNumberOfChildren() const
{
    return children.size();
}

ScreenElement* ScreenLayoutElement::getChild( int i )
{
    return children[i];
}

const ScreenElement* ScreenLayoutElement::getChild( int i ) const
{
    return children[i];
}

void ScreenLayoutElement::addChild( ScreenElement* child )
{
    children.push_back( child );
}

void ScreenLayoutElement::deleteChild( ScreenElement* child )
{
    for ( std::vector<ScreenElement*>::iterator i = children.begin();
          i != children.end(); ++i )
        if ( *i == child )
        {
            children.erase( i );
            break;
        }
}

bool ScreenLayoutElement::operator==(const ScreenLayoutElement& other) const
{
    return  ScreenElement::operator==(other) &&
            ApexTools::haveSameContents(children, other.children);
}

}
}

