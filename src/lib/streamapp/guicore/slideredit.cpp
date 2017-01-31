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
 
#include "slideredit.h"
#include "guicore/customlayout.h"
#include "appcore/qt_utils.h"
#include "utils/dataconversion.h"
using namespace dataconversion;

#if QT_VERSION < 0x040000
#include <qslider.h>
#else
#include <QSlider>
#endif

using namespace guicore;

SliderEdit::SliderEdit( QWidget* a_pParent, const Qt::Orientation ac_Orient, const guicore::FriendLayout::mt_eFriendPosition ac_eWhere ) :
    LineEdit( a_pParent, "" ),
  mc_pButton( new QSlider( a_pParent ) ),
  mc_pLayout( new FriendLayout( mc_pButton, (QWidget*) this, ac_eWhere, 0.2 ) ),
  mv_dDivider( 1.0 ),
  mv_bEmitDouble( true )
{
  mc_pButton->show();
  mc_pButton->setOrientation( ac_Orient );
  connect( (QObject*)mc_pButton, SIGNAL( valueChanged(int) ), SLOT( mp_SliderChanged(int) ) );
  connect( (QObject*)mc_pButton, SIGNAL( sliderReleased() ), SLOT( mp_SliderReleased() ) );
  connect( this, SIGNAL( returnPressed() ), SLOT( mp_EditEntered() ) );
}

SliderEdit::~SliderEdit()
{
  delete mc_pButton;
  delete mc_pLayout;
}

void SliderEdit::setGeometry( const QRect &ac_Rect )
{
  static bool s_bRecurseMe = true;
  if( s_bRecurseMe )
  {
    s_bRecurseMe = false;
    mc_pLayout->mp_ResizeThemAll( ac_Rect );
    s_bRecurseMe = true;
  }
  else
    QLineEdit::setGeometry( ac_Rect );
}

void SliderEdit::mp_SetValue( const double ac_dValue )
{
  mc_pButton->setValue( roundDoubleToInt( ac_dValue * mv_dDivider ) );
  setText( qn( ac_dValue ) );
}

void SliderEdit::mp_SetValueSignaling( const double ac_dValue )
{
  mc_pButton->setValue( roundDoubleToInt( ac_dValue * mv_dDivider ) );
  setText( qn( ac_dValue ) );
  mp_SliderReleased();
}

void SliderEdit::mp_SetPosition( const int ac_nPos )
{
  mc_pButton->setValue( roundDoubleToInt( (double) ac_nPos * mv_dDivider ) );
  mp_EditChanged();
}

void SliderEdit::mp_SetSliderStuff( const int ac_nMin, const int ac_nMax, const int ac_nStep )
{
#if QT_VERSION < 0x040000
  mc_pButton->setLineStep( ac_nStep );
  mc_pButton->setMinValue( ac_nMin );
  mc_pButton->setMaxValue( ac_nMax );
#else
  mc_pButton->setSingleStep( ac_nStep );
  mc_pButton->setMinimum( ac_nMin );
  mc_pButton->setMaximum( ac_nMax );
#endif
}

void SliderEdit::mp_SetTracking( const bool ac_bSet )
{
  mc_pButton->setTracking( ac_bSet );
}

void SliderEdit::mp_SliderReleased()
{
  if( mv_bEmitDouble )
    emit ms_ValueChange( LineEdit::text().toDouble() );
  else
    emit ms_ValueChange( LineEdit::text() );
}

void SliderEdit::mp_SliderChanged( int a_nVal )
{
  const double dNewVal = (double) a_nVal / mv_dDivider;
  LineEdit::setText( qn( dNewVal ) );
}

void SliderEdit::mp_EditChanged()
{
  mc_pButton->setValue( roundDoubleToInt( LineEdit::text().toDouble() * mv_dDivider ) );
}

void SliderEdit::mp_EditEntered()
{
  mp_EditChanged();
  mp_SliderReleased();
}
