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
 
#include "emptyelement.h"

#include "screenelementvisitor.h"

namespace apex
{
namespace data
{

  EmptyElement::EmptyElement( const QString& id, ScreenElement* parent )
    : ScreenElement( id, parent )
{
}

void EmptyElement::visit( ScreenElementVisitor* v )
{
    v->visitEmpty( this );
}

void EmptyElement::visit( ScreenElementVisitor* v ) const
{
    v->visitEmpty( this );
}

QString EmptyElement::findFreeID( const ScreenElementMap& takenIDs )
{
    int i = 0;
    while ( true ) {
        QString testId = QString( "empty%1" ).arg( i++ );
        if ( takenIDs.find( testId ) == takenIDs.end() )
            return testId;
    }
}

ScreenElement::ElementTypeT EmptyElement::elementType() const
{
    return Empty;
}

}
}


