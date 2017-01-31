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
 
#include "screen/screenrundelegate.h"
#include "gui/guidefines.h"

#include "screen/checkboxelement.h"
#include "screen/checkboxrundelegate.h"

#include "apextools.h"

#include <QColor>
#include <QPalette>

namespace apex
{
namespace rundelegates
{

const ScreenElement* CheckBoxRunDelegate::getScreenElement() const
{
    return element;
}

QWidget* CheckBoxRunDelegate::getWidget()
{
    return this;
}

bool CheckBoxRunDelegate::hasText() const
{
    return true;
}

bool CheckBoxRunDelegate::hasInterestingText() const
{
    return true;
}

const QString CheckBoxRunDelegate::getText() const
{
	bool state = (checkState() == Qt::Checked);
	return QString::number(state);
}

void CheckBoxRunDelegate::resizeEvent( QResizeEvent* e )
{
    QCheckBox::resizeEvent (e);
    setFont (initialFont);
    ApexTools::shrinkTillItFits (this, getText(), QSize (2, 2));

  //QCheckBox::setCheckState(checkState());
}

void CheckBoxRunDelegate::connectSlots( gui::ScreenRunDelegate* d )
{
    connect( this, SIGNAL( answered( ScreenElementRunDelegate* ) ),
             d, SIGNAL( answered( ScreenElementRunDelegate* ) ) );
}

}
}

void apex::rundelegates::CheckBoxRunDelegate::sendAnsweredSignal()
{
    emit answered( this );
}

apex::rundelegates::CheckBoxRunDelegate::CheckBoxRunDelegate(
        ExperimentRunDelegate* p_exprd,
    QWidget* parent, const CheckBoxElement* e, const QFont& defaultFont ) :
      QCheckBox( parent ),
      ScreenElementRunDelegate(p_exprd, e),
      element( e )
{
   QCheckBox::setText( e->getText() );

   QFont font = defaultFont;
   if ( element->getFontSize() != -1 )
       font.setPointSize( element->getFontSize() );
   QCheckBox::setFont( font );

   QPalette plt;
   plt.setColor(QPalette::WindowText, Qt::red);
   QCheckBox::setPalette(plt);
   initialFont = font;

   QCheckBox::setCheckState(checkState());
}

