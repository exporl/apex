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

#include "parameterscontainerelement.h"

#include <QRegExp>
#include <QStringList>

namespace apex
{
namespace data
{

ParametersContainerElement::ParametersContainerElement(const QString &id,
                                                       ScreenElement *parent)
    : ScreenElement(id, parent)
{
}

ParametersContainerElement::~ParametersContainerElement()
{
}

bool ParametersContainerElement::CheckExpression(const QString &p_expr)
{
    if (p_expr.isEmpty())
        return true;

    static QRegExp re(parameterExpressionRegexpData);
    return re.exactMatch(p_expr);
}

const char ParametersContainerElement::parameterExpressionRegexpData[] =
    "^(-?\\d*\\.?\\d*)\\s*\\*\\s*x\\s*([+-])\\s*(\\d+\\.?\\d*)$";
}
}
