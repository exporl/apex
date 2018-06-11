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

#include "flashplayerelement.h"

#include "screenelementvisitor.h"

namespace apex
{
namespace data
{

FlashPlayerElement::FlashPlayerElement(const QString &id, ScreenElement *parent)
    : ScreenElement(id, parent), overrideFeedback(false)
{
}

FlashPlayerElement::~FlashPlayerElement()
{
}

QString FlashPlayerElement::getDefault() const
{
    return defaultPath;
}

QString FlashPlayerElement::getHighlight() const
{
    return highlightPath;
}

QString FlashPlayerElement::getPositive() const
{
    return positivePath;
}

QString FlashPlayerElement::getNegative() const
{
    return negativePath;
}

void FlashPlayerElement::setDefault(const QString &p)
{
    defaultPath = p;
}

void FlashPlayerElement::setPositive(const QString &p)
{
    positivePath = p;
}

void FlashPlayerElement::setNegative(const QString &p)
{
    negativePath = p;
}

void FlashPlayerElement::setHighlight(const QString &p)
{
    highlightPath = p;
}

void FlashPlayerElement::visit(ScreenElementVisitor *v)
{
    v->visitFlashPlayer(this);
}

void FlashPlayerElement::visit(ScreenElementVisitor *v) const
{
    v->visitFlashPlayer(this);
}

bool FlashPlayerElement::getOverrideFeedback() const
{
    return overrideFeedback;
}

void FlashPlayerElement::setOverrideFeedback(bool o)
{
    overrideFeedback = o;
}

ScreenElement::ElementTypeT FlashPlayerElement::elementType() const
{
    return FlashPlayer;
}

bool FlashPlayerElement::operator==(const FlashPlayerElement &other) const
{
    return ScreenElement::operator==(other) &&
           defaultPath == other.defaultPath &&
           highlightPath == other.highlightPath &&
           positivePath == other.positivePath &&
           negativePath == other.negativePath &&
           overrideFeedback == other.overrideFeedback;
}
}
}
