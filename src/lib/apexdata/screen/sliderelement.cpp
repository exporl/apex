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

#include "sliderelement.h"

#include "screenelementvisitor.h"

#include <QObject>
//#include <QtGui>

namespace apex
{
namespace data
{

SliderElement::SliderElement( const QString& id, ScreenElement* parent )
    : ScreenElement( id, parent ),
     hasDefault(true),
         orient(Qt::Vertical),
         value(0),
         minimum(-20),
         maximum(20),
         tickPos(/*QSlider::TicksBothSides*/3), //FIXME
         stepSize(1),
         pageSize(5),
         tickInt(0)
{
}

SliderElement::~SliderElement()
{
}

/*
void SliderElement::setText( const QString& t )
{
    text = t;
}

QString SliderElement::getText() const
{
    return text;
}
*/
void SliderElement::visit( ScreenElementVisitor* v )
{
    v->visitSlider( this );
}

void SliderElement::visit( ScreenElementVisitor* v ) const
{
    v->visitSlider( this );
}

ScreenElement::ElementTypeT SliderElement::elementType() const
{
    return Slider;
}

bool SliderElement::operator==(const SliderElement& other) const
{
    return  ScreenElement::operator==(other) &&
            text == other.text &&
            hasDefault == other.hasDefault &&
            orient == other.orient &&
            value == other.value &&
            minimum == other.minimum &&
            maximum == other.maximum &&
            tickPos == other.tickPos &&
            stepSize == other.stepSize &&
            pageSize == other.pageSize &&
            tickInt == other.tickInt &&
            reset == other.reset &&
            parameter == other.parameter;
}

}
}

