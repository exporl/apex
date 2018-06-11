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

#include "labelelement.h"

#include "screenelementvisitor.h"

#include <QObject>

namespace apex
{
namespace data
{

void LabelElement::setText(const QString &t)
{
    text = t;
}

LabelElement::~LabelElement()
{
}

LabelElement::LabelElement(const QString &id, ScreenElement *parent)
    : ScreenElement(id, parent), text(QObject::tr("<No text set>"))
{
}

QString LabelElement::getText() const
{
    return text;
}

void LabelElement::visit(ScreenElementVisitor *v)
{
    v->visitLabel(this);
}

void LabelElement::visit(ScreenElementVisitor *v) const
{
    v->visitLabel(this);
}

ScreenElement::ElementTypeT LabelElement::elementType() const
{
    return Label;
}

bool LabelElement::operator==(const LabelElement &other) const
{
    return ScreenElement::operator==(other) && text == other.text;
}
}
}
