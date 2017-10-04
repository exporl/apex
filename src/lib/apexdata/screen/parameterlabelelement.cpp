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

#include "apextools/exceptions.h"

#include "parameterlabelelement.h"
#include "screenelementvisitor.h"

namespace apex
{
namespace data
{

ParameterLabelElement::ParameterLabelElement(const QString &id,
                                             ScreenElement *parent)
    : ParametersContainerElement(id, parent)
{
}

ParameterLabelElement::~ParameterLabelElement()
{
}

ScreenElement::ElementTypeT ParameterLabelElement::elementType() const
{
    return ParameterLabel;
}

void ParameterLabelElement::visit(ScreenElementVisitor *v)
{
    v->visitParameterLabel(this);
}

void ParameterLabelElement::visit(ScreenElementVisitor *v) const
{
    v->visitParameterLabel(this);
}

bool ParameterLabelElement::setParameter(ParameterData d)
{
    if (!CheckExpression(d.expression))
        throw ApexStringException(
            QString(tr("Invalid expression %1 in parameterlabel %2"))
                .arg(d.expression)
                .arg(getID()));
    parameterData = d;

    return true;
}

ParameterData ParameterLabelElement::getParameter() const
{
    return parameterData;
}

bool ParameterLabelElement::operator==(const ParameterLabelElement &other) const
{
    return ScreenElement::operator==(other) &&
           parameterData == other.parameterData;
}

} // ns data
} // ns apex
