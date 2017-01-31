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

#include "spinboxelement.h"

#include "screenelementvisitor.h"

#include <QObject>

namespace apex
{
namespace data
{

SpinBoxElement::SpinBoxElement( const QString& id, ScreenElement* parent )
    : ScreenElement( id, parent ),
    hasDefault(false),
       minimum(-100),
        maximum(100),
        step(1),
        value(0),
        reset(false)
{
}

SpinBoxElement::~SpinBoxElement()
{
}


void SpinBoxElement::visit( ScreenElementVisitor* v )
{
    v->visitSpinBox( this );
}

void SpinBoxElement::visit( ScreenElementVisitor* v ) const
{
    v->visitSpinBox( this );
}

ScreenElement::ElementTypeT SpinBoxElement::elementType() const
{
    return SpinBox;
}

bool SpinBoxElement::operator==(const SpinBoxElement& other) const
{
    return  ScreenElement::operator==(other) &&
            hasDefault == other.hasDefault &&
            minimum == other.minimum &&
            maximum == other.maximum &&
            step == other.step &&
            value == other.value &&
            reset == other.reset &&
            parameter == other.parameter;
}

}
}

