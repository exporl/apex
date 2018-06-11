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

#include "texteditelement.h"

#include "screenelementvisitor.h"

namespace apex
{
namespace data
{

TextEditElement::TextEditElement(const QString &id, ScreenElement *parent)
    : ScreenElement(id, parent)
{
}

TextEditElement::~TextEditElement()
{
}

void TextEditElement::setText(const QString &t)
{
    text = t;
}

const QString TextEditElement::getText() const
{
    return text;
}

void TextEditElement::setInputMask(const QString &t)
{
    inputMask = t;
}

const QString TextEditElement::getInputMask() const
{
    return inputMask;
}

void TextEditElement::visit(ScreenElementVisitor *v)
{
    v->visitTextEdit(this);
}

void TextEditElement::visit(ScreenElementVisitor *v) const
{
    v->visitTextEdit(this);
}

ScreenElement::ElementTypeT TextEditElement::elementType() const
{
    return TextEdit;
}

bool TextEditElement::operator==(const TextEditElement &other) const
{
    return ScreenElement::operator==(other) && text == other.text &&
           inputMask == other.inputMask;
}
}
}
