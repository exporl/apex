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

#include "pictureelement.h"
#include "parameterscontainerelement.h"

#include "screenelementvisitor.h"

namespace apex
{
namespace data
{

PictureElement::PictureElement(
    const QString& id, ScreenElement* parent)
        : ScreenElement(id, parent),
        overrideFeedback(false)
{
}

PictureElement::~PictureElement()
{
}

QString PictureElement::getDefault() const
{
    return defaultPath;
}

QString PictureElement::getHighlight() const
{
    return highlightPath;
}

QString PictureElement::getPositive() const
{
    return positivePath;
}

QString PictureElement::getNegative() const
{
    return negativePath;
}

QString PictureElement::getDisabled() const
{
    return disabledPath;
}

QString PictureElement::getUriId() const
{
       return uriId;
}

void PictureElement::setDefault(const QString& p)
{
    defaultPath = p;
}

void PictureElement::setUriId(const QString &p)
{
    uriId = p;
}

void PictureElement::setPositive(const QString& p)
{
    positivePath = p;
}

void PictureElement::setNegative(const QString& p)
{
    negativePath = p;
}

void PictureElement::setHighlight(const QString& p)
{
    highlightPath = p;
}

void PictureElement::setDisabled(const QString& p)
{
    disabledPath = p;
}

bool PictureElement::getOverrideFeedback() const
{
    return overrideFeedback;
}

void PictureElement::setOverrideFeedback(bool o)
{
    overrideFeedback = o;
}

void PictureElement::visit(ScreenElementVisitor* v)
{
    v->visitPicture(this);
}

void PictureElement::visit(ScreenElementVisitor* v) const
{
    v->visitPicture(this);
}

ScreenElement::ElementTypeT PictureElement::elementType() const
{
    return Picture;
}

bool PictureElement::operator==(const PictureElement& other) const
{
    return  ScreenElement::operator==(other) &&
            defaultPath == other.defaultPath &&
            highlightPath == other.highlightPath &&
            positivePath == other.positivePath &&
            negativePath == other.negativePath &&
            overrideFeedback == other.overrideFeedback;
}

}
}

