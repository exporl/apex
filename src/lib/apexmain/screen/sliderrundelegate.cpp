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

#include "apexdata/screen/sliderelement.h"

#include "apextools/apextools.h"

#include "gui/guidefines.h"

#include "screen/screenrundelegate.h"
#include "screen/sliderrundelegate.h"

namespace apex
{
namespace rundelegates
{

const ScreenElement *SliderRunDelegate::getScreenElement() const
{
    return element;
}

QWidget *SliderRunDelegate::getWidget()
{
    return this;
}

bool SliderRunDelegate::hasText() const
{
    return true;
}

bool SliderRunDelegate::hasInterestingText() const
{
    return true;
}

const QString SliderRunDelegate::getText() const
{
    int sliderValue = QSlider::value();
    return QString::number(sliderValue);
}

void SliderRunDelegate::resizeEvent(QResizeEvent *e)
{
    QSlider::resizeEvent(e);
    setFont(initialFont);
    ApexTools::shrinkTillItFits(this, getText(), QSize(2, 2));

    if (element->GetReset()) {
        if (element->HasDefault()) {
            qCDebug(APEX_RS, "Reset me!!!!\n");
            QSlider::setValue((int)element->GetValue());
        } else {
            QSlider::setValue(0);
        }
    }
}

void SliderRunDelegate::connectSlots(gui::ScreenRunDelegate *d)
{
    connect(this, SIGNAL(answered(ScreenElementRunDelegate *)), d,
            SIGNAL(answered(ScreenElementRunDelegate *)));
}
}
}

void apex::rundelegates::SliderRunDelegate::sendAnsweredSignal()
{
    Q_EMIT answered(this);
}

apex::rundelegates::SliderRunDelegate::SliderRunDelegate(
    ExperimentRunDelegate *p_exprd, QWidget *parent, const SliderElement *e,
    const QFont &defaultFont)
    : QSlider(e->GetOrient(), parent),
      ScreenElementRunDelegate(p_exprd, e),
      element(e)
{
    /*QSlider::setText( e->getText() );
    if ( !element->getShortCut().isEmpty() )
        QSlider::setAccel( e->getShortCut() );

        connect( this, SIGNAL( valueChanged(int) ), this, SLOT(
    sendAnsweredSignal() ) );*/

    QFont font = defaultFont;
    if (element->getFontSize() != -1)
        font.setPointSize(element->getFontSize());
    QSlider::setFont(font);
    initialFont = font;

    QSlider::setMinimum((int)element->GetMin());
    QSlider::setMaximum((int)element->GetMax());
    if (element->HasDefault()) {
        QSlider::setValue((int)element->GetValue());
    }
    QSlider::setTickPosition((QSlider::TickPosition)element->GetTickPos());
    QSlider::setTickInterval((int)element->GetTickInt());
    QSlider::setSingleStep(element->GetStepSize());
    QSlider::setPageStep(element->GetPageSize());
}
