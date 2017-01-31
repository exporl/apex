/******************************************************************************
 * Copyright (C) 2008  Job Noorman <jobnoorman@gmail.com>                     *
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

#include "decibellineedit.h"

#include <QDoubleValidator>
#include <QFocusEvent>
#include <QDebug>

#include <cmath>            // pow

using namespace spin::gui;

DecibelLineEdit::DecibelLineEdit(QWidget* parent) :
    QLineEdit(parent),
    emitting(false)
{
    QDoubleValidator* v = new QDoubleValidator(-200, 200, 2, parent);
    v->setNotation(QDoubleValidator::StandardNotation);
    setValidator(v);
    setFixedWidth(calculateSize().width());

    connect(this, SIGNAL(textEdited(QString)),
            this, SLOT(emitLevelChange(QString)));
}

void DecibelLineEdit::setLevel(double level)
{
    if (this->level() != level )
    {
    //    qCDebug(APEX_RS, "Setting level to %f", level);
        //get the nb of decimals of the validator
        const QDoubleValidator* v =
                qobject_cast<const QDoubleValidator*>(validator());
        int decimals = v->decimals();

        //check if the level is not smaller than the smallest possible value
        /*
        double smallestValue = 0.1;

        for (int i = 1; i < decimals; i++)
            smallestValue /= 10;

        if (level < smallestValue)
            level = 0.0;

        setText(QString::number(level, 'g', decimals + 2));//FIXME*/
        // round to correct number of decimal places
                level = qRound(level*std::pow(10.0,decimals))/std::pow(10.0,decimals);
        setText(QString::number(level));
    }
}

double DecibelLineEdit::level() const
{
    if (!hasLevel())
        return 0;
    return text().toDouble();
}

bool DecibelLineEdit::hasLevel() const
{
    return  hasAcceptableInput();
}

QSize DecibelLineEdit::calculateSize()
{
    const QDoubleValidator* v =
            qobject_cast<const QDoubleValidator*>(validator());

    QString longestString = QString::number(v->top());
    longestString += ".";

    for (int i = 0; i < v->decimals(); i++)
        longestString += "9";

    return QSize(fontMetrics().width(longestString) + 15,
                 0);
}

void DecibelLineEdit::emitLevelChange(QString levelText)
{
    Q_UNUSED(levelText);
    if (emitting)
        return;

    //start emitting
    emitting = true;

    if (hasLevel())
        emit levelChanged(level());

    //done emitting
    emitting = false;
}

void DecibelLineEdit::focusOutEvent(QFocusEvent* event)
{
    if (!text().isEmpty() && !hasAcceptableInput())
        setText(QString::number(0.0));

    emit levelChanged(level());
    QLineEdit::focusOutEvent(event);
}

