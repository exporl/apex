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

#include "buttonelement.h"

#include "screenelementvisitor.h"

#include <QObject>

namespace apex
{
namespace data
{

ButtonElement::ButtonElement( const QString& id, ScreenElement* parent )
    : ScreenElement( id, parent ),
      txt( QObject::tr( "<No text set>" ) )
{
}

ButtonElement::~ButtonElement()
{
}

void ButtonElement::setText( const QString& t )
{
    txt = t;
}

QString ButtonElement::text() const
{
    return txt;
}

void ButtonElement::visit( ScreenElementVisitor* v )
{
    v->visitButton( this );
}

void ButtonElement::visit( ScreenElementVisitor* v ) const
{
    v->visitButton( this );
}

ScreenElement::ElementTypeT ButtonElement::elementType() const
{
    return Button;
}

bool ButtonElement::operator==(const ButtonElement& other) const
{
    return ScreenElement::operator==(other) && txt == other.txt;
}

}
}

