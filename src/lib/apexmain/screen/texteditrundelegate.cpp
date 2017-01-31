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
 
#include "texteditrundelegate.h"

#include "screen/texteditelement.h"
#include "gui/guidefines.h"
#include "screen/screenrundelegate.h"

#include <QIntValidator>

namespace apex
{
namespace rundelegates
{

using gui::sc_DefaultPanelColor;

TextEditRunDelegate::TextEditRunDelegate(
    ExperimentRunDelegate* p_exprd,
    QWidget* parent, const TextEditElement* e,
    const QFont& defaultFont )
        : QLineEdit( parent ),
        ScreenElementRunDelegate(p_exprd, e),
        element( e )
{
    setObjectName(element->getID());
    setStyleSheet(element->getStyle());
    setText( e->getText() );
    connect( this, SIGNAL( returnPressed() ),
             this, SLOT( sendAnsweredSignal() ) );
    //no idea which color does what, so just set the background and default the rest
/*    QPalette* p = guicore::widgetattributes::gf_pInitPaletteWithBGColor( sc_DefaultPanelColor );
    setPalette( *p );
    delete p;*/

    QFont font = defaultFont;
    if ( element->getFontSize() != -1 )
        font.setPointSize( element->getFontSize() );
    QLineEdit::setFont( font );

    QString sMask = e->getInputMask();
    if ( sMask == "numbers" )
        setValidator( new QIntValidator( -32768, 32767, this ) );
    else
        setInputMask( sMask );
}

const ScreenElement* TextEditRunDelegate::getScreenElement() const
{
    return element;
}

QWidget* TextEditRunDelegate::getWidget()
{
    return this;
}

bool TextEditRunDelegate::hasText() const
{
    return true;
}

bool TextEditRunDelegate::hasInterestingText() const
{
    return true;
}

const QString TextEditRunDelegate::getText() const
{
    return text();
}

void TextEditRunDelegate::sendAnsweredSignal()
{
    emit answered( this );
}

void TextEditRunDelegate::setText( const QString& t )
{
    QLineEdit::setText( t );
    if (! inputMask().isEmpty())
        home(true);
}

void TextEditRunDelegate::clearText()
{
    setText( element->getText() );
}

}
}

void apex::rundelegates::TextEditRunDelegate::connectSlots( gui::ScreenRunDelegate* d )
{
    connect( this, SIGNAL( answered( ScreenElementRunDelegate* ) ),
             d, SIGNAL( answered( ScreenElementRunDelegate* ) ) );
}

