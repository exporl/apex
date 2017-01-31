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

#include "apexdata/screen/screenelement.h"

#include "apextools/apextools.h"

#include "labelrundelegatebase.h"

apex::rundelegates::LabelRunDelegateBase::LabelRunDelegateBase(
        ExperimentRunDelegate* p_exprd,
    QWidget* parent, const ScreenElement* screenEl, const QFont& defaultFont ) :
        QLabel( parent ),
        ScreenElementRunDelegate(p_exprd, screenEl)
{
    setAlignment (Qt::AlignCenter);
    setAutoFillBackground( true );
    setFrameShape( QLabel::Box );
    setBackgroundRole( QPalette::AlternateBase );

    /*setStyleSheet(screenEl->getStyle());
    setBgColor(this, screenEl->getBGColor());
    setFgColor(this, screenEl->getFGColor());*/
    setCommonProperties(this);

    QFont font = defaultFont;
    if ( screenEl->getFontSize() != -1 )
        font.setPointSize( screenEl->getFontSize() );
    QLabel::setFont( font );
    initialFont = font;
}

QLabel* apex::rundelegates::LabelRunDelegateBase::getWidget()
{
    return this;
}

bool apex::rundelegates::LabelRunDelegateBase::hasText() const
{
    return true;
}

bool apex::rundelegates::LabelRunDelegateBase::hasInterestingText() const
{
    return false;
}

const QString apex::rundelegates::LabelRunDelegateBase::getText() const
{
    return text;
}

void apex::rundelegates::LabelRunDelegateBase::resizeEvent( QResizeEvent* e)
{
    QLabel::resizeEvent (e);
    setFont (initialFont);
    ApexTools::shrinkTillItFits (this, text, QSize (2, 2));
}

void apex::rundelegates::LabelRunDelegateBase::setEnabled( const bool )
{
    // can't be disabled
    QLabel::setEnabled( true );
}

void apex::rundelegates::LabelRunDelegateBase::setText( const QString& t )
{
    text = t;
    QLabel::setText (text);
    setFont (initialFont);
    ApexTools::shrinkTillItFits (this, text, QSize (2, 2));
}

