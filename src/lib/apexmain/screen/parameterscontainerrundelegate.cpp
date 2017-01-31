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
 
#include "parameterscontainerrundelegate.h"

//from libdata
#include "screen/parameterscontainerelement.h"

#include <QRegExp>
#include <QStringList>

namespace apex
{
namespace rundelegates
{

ParametersContainerRunDelegate::ParametersContainerRunDelegate( ) {

}

ParametersContainerRunDelegate::~ParametersContainerRunDelegate()
{}



double ParametersContainerRunDelegate::ParseExpression(
    const QString& p_expr, double value)
{
    QRegExp re( ParametersContainerElement::parameterExpressionRegexpData );

    bool re_exactMatch = re.exactMatch(p_expr);
    Q_UNUSED(re_exactMatch);
    Q_ASSERT( re_exactMatch );

    QStringList list = re.capturedTexts();
    double a;
    if ((list.at(1))=="-") a=-1;
    else a= (list.at(1)).toDouble();
    QString sign = list.at(2);
    double b= list.at(3).toDouble();
#ifdef PRINTPARAMETERLIST
    qDebug("expr=" + p_expr);
    qDebug("list(1)=" + list.at(1));
    qDebug("list(2)=" + list.at(2));
    qDebug("list(3)=" + list.at(3));

    qDebug("a=" + QString::number(a) + ", b=" + QString::number(b) + ", sign=" + sign);
#endif

    if (sign=="-")
        b=-b;
    return a*value+b;
}




}
}
