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
 
#include "picturelabelelement.h"

#include "screenelementvisitor.h"

namespace apex
{
namespace data
{

PictureLabelElement::PictureLabelElement( const QString& id, ScreenElement* parent )
    : ScreenElement( id, parent )
{
}

PictureLabelElement::~PictureLabelElement()
{
}

void PictureLabelElement::setPicture( const QString& p )
{
    picturePath = p;
}

void PictureLabelElement::setPictureDisabled( const QString& p )
{
    pictureDisabledPath = p;
}

QString PictureLabelElement::getPicture() const
{
    return picturePath;
}

QString PictureLabelElement::getPictureDisabled() const
{
    return pictureDisabledPath;
}

void PictureLabelElement::visit( ScreenElementVisitor* v )
{
    v->visitPictureLabel( this );
}

void PictureLabelElement::visit( ScreenElementVisitor* v ) const
{
    v->visitPictureLabel( this );
}

ScreenElement::ElementTypeT PictureLabelElement::elementType() const
{
    return PictureLabel;
}

bool PictureLabelElement::operator==(const PictureLabelElement& other) const
{
    return ScreenElement::operator==(other) && picturePath == other.picturePath;
}

}
}

