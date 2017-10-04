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

#include "checkboxelement.h"

#include "screenelementvisitor.h"

#include <QObject>

namespace apex
{
namespace data
{

CheckBoxElement::CheckBoxElement(const QString &id, ScreenElement *parent)
    : ScreenElement(id, parent),
      text(QObject::tr("<No text set>")),
      isChecked(Qt::Unchecked)
{
}

CheckBoxElement::~CheckBoxElement()
{
}

void CheckBoxElement::setText(const QString &t)
{
    text = t;
}

QString CheckBoxElement::getText() const
{
    return text;
}

Qt::CheckState CheckBoxElement::getChecked() const
{
    return isChecked;
}

void CheckBoxElement::visit(ScreenElementVisitor *v)
{
    v->visitCheckBox(this);
}

void CheckBoxElement::visit(ScreenElementVisitor *v) const
{
    v->visitCheckBox(this);
}

ScreenElement::ElementTypeT CheckBoxElement::elementType() const
{
    return CheckBox;
}

bool CheckBoxElement::operator==(const CheckBoxElement &other) const
{
    return ScreenElement::operator==(other) && text == other.text;
}
}
}
