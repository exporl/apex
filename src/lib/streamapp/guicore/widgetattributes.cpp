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
 
#include "widgetattributes.h"
#include "customlayout.h"
#include "appcore/qt_utils.h"

#if QT_VERSION < 0x040000
#include <qlabel.h>
#include <qwidget.h>
#include <qapplication.h>
#include <qfontmetrics.h>
#else
#include <QLabel>
#include <QWidget>
#include <QColorGroup>
#include <QApplication>
#include <QFontMetrics>
#endif

using namespace guicore;

void widgetattributes::gf_SetBackGroundColor( QWidget* a_pVictim, const QColor& ac_Color )
{
#if QT_VERSION < 0x040000
  a_pVictim->setPaletteBackgroundColor( ac_Color );
#else
  QPalette pal = a_pVictim->palette();
  pal.setColor( a_pVictim->backgroundRole(), ac_Color );
  a_pVictim->setPalette( pal );
#endif
}

void widgetattributes::gf_SetForeGroundColor( QWidget* a_pVictim, const QColor& ac_Color )
{
#if QT_VERSION < 0x040000
  a_pVictim->setPaletteForegroundColor( ac_Color );
#else
  QPalette pal = a_pVictim->palette();
  pal.setColor( a_pVictim->foregroundRole(), ac_Color );
  a_pVictim->setPalette( pal );
#endif
}

void widgetattributes::gf_ShrinkTillItFits( QWidget* a_pVictim, const QString& ac_sText, const int ac_nWidthBorder, const int ac_nHeightBorder )
{
  QFontMetrics metr( a_pVictim->fontMetrics() );
  QFont fontr( a_pVictim->font() );
  const int nW = a_pVictim->width() - ( ac_nWidthBorder * 2 );
  const int nH = a_pVictim->height() - ( ac_nHeightBorder * 2 );

    //QFontMetrics doesn't know about line so split the text
    //and find the longest line
  QStringList lines( f_SplitStringInLines( ac_sText ) );
  const QStringList::size_type nLines = lines.size();
  unsigned nLongest = 0;
  unsigned nLongestLen = 0;
  for( QStringList::size_type i = 0 ; i < nLines ; ++i )
  {
    const unsigned nLen = lines[ i ].length();
    if( nLen > nLongestLen )
    {
      nLongest = (unsigned) i;
      nLongestLen = nLen;
    }
  }

  const QString& text( lines[ nLongest ] );

  while( metr.width( text ) > nW )
  {
    const int nNewSiz = fontr.pointSize() - 1;
    if( nNewSiz <= 1 )
      break;
    fontr.setPointSize( nNewSiz );
    a_pVictim->setFont( fontr );
    metr = a_pVictim->fontMetrics();
  }
#if QT_VERSION < 0x040000
  while( ( metr.height() * nLines ) > (unsigned) nH )
#else
  while( ( metr.height() * nLines ) > nH )
#endif
  {
    const int nNewSiz = fontr.pointSize() - 1;
    if( nNewSiz <= 1 )
      break;
    fontr.setPointSize( nNewSiz );
    a_pVictim->setFont( fontr );
    metr = a_pVictim->fontMetrics();
  }
}

void widgetattributes::gf_ApplyDefaultAlignment( QLabel* a_pVictim )
{
#if QT_VERSION < 0x040000
  a_pVictim->setAlignment( QLabel::AlignHCenter | QLabel::AlignVCenter );
#else
  a_pVictim->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
#endif
}

void widgetattributes::gf_ApplyPhatText( QLabel* a_pVictim, const int ac_nHowPhat )
{
  QFont a_pVictimLabelfont(  a_pVictim->font() );
  a_pVictimLabelfont.setFamily( "Courier New" );
  a_pVictimLabelfont.setPointSize( ac_nHowPhat );
  a_pVictimLabelfont.setBold( TRUE );
  a_pVictim->setFont( a_pVictimLabelfont );
}

void widgetattributes::gf_SizeToDesktop( QWidget* a_pVictim, const double ac_dXSize, const double ac_dYSize )
{
  QRect rork( ( (QWidget*) QApplication::desktop() )->rect() );
  if( ac_dXSize != 1.0 )
  {
    unsigned nOldW = rork.width();        //FIXME these are 'shrinkwidth' and shrinkheight'
    unsigned nOldH = rork.height();
    rork.setWidth( (int) (nOldW * ac_dXSize) );
    rork.setHeight( (int) (nOldH * ac_dYSize) );
    a_pVictim->resize( QSize( rork.width(), rork.height() ) );
    //rork.moveBy( )
  }
}

void widgetattributes::gf_ScaleToDesktop( QWidget* a_pVictim, tItemLayout& ac_Layout )
{
  CustomLayout* lay = new CustomLayout();
  lay->mp_AddItem( a_pVictim, &ac_Layout );
  lay->mp_ResizeThemAll( ( (QWidget*) QApplication::desktop() )->rect() );
}

void widgetattributes::gf_SetFixedSize( QWidget* a_pVictim, const unsigned ac_nWidth, const unsigned ac_nHeight )
{
  a_pVictim->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
  a_pVictim->setMaximumWidth ( ac_nWidth  );
  a_pVictim->setMinimumWidth ( ac_nWidth  );
  a_pVictim->setMaximumHeight( ac_nHeight );
  a_pVictim->setMinimumHeight( ac_nHeight );
}

namespace guicore
{
  namespace widgetattributes
  {
    const QPalette* sc_pPalette = 0;
  }
}

void widgetattributes::gf_SetIndicatorPalette( const QPalette* ac_pPalette )
{
  sc_pPalette = ac_pPalette;
}

const QPalette* widgetattributes::gf_pGetIndicatorPalette()
{
  return sc_pPalette;
}

QPalette* widgetattributes::gf_pInitPaletteWithBGColor( const QColor& ac_DisabledBackGroundColor )
{
  QPalette* pal = new QPalette();
#if QT_VERSION < 0x040000
  QColorGroup cg;
  cg.setColor( QColorGroup::Foreground, Qt::black );
  cg.setColor( QColorGroup::Button, QColor( 212, 208, 200) );
  cg.setColor( QColorGroup::Light, Qt::white );
  cg.setColor( QColorGroup::Midlight, QColor( 233, 231, 227) );
  cg.setColor( QColorGroup::Dark, QColor( 106, 104, 100) );
  cg.setColor( QColorGroup::Mid, QColor( 141, 138, 133) );
  cg.setColor( QColorGroup::Text, Qt::black );
  cg.setColor( QColorGroup::BrightText, Qt::white );
  cg.setColor( QColorGroup::ButtonText, Qt::black );
  cg.setColor( QColorGroup::Base, Qt::white );
  cg.setColor( QColorGroup::Background, ac_DisabledBackGroundColor );
  cg.setColor( QColorGroup::Shadow, Qt::black );
  cg.setColor( QColorGroup::Highlight, QColor( 0, 0, 128) );
  cg.setColor( QColorGroup::HighlightedText, Qt::white );
  cg.setColor( QColorGroup::Link, Qt::black );
  cg.setColor( QColorGroup::LinkVisited, Qt::black );
  pal->setActive( cg );
  cg.setColor( QColorGroup::Foreground, Qt::black );
  cg.setColor( QColorGroup::Button, QColor( 212, 208, 200) );
  cg.setColor( QColorGroup::Light, Qt::white );
  cg.setColor( QColorGroup::Midlight, QColor( 243, 239, 230) );
  cg.setColor( QColorGroup::Dark, QColor( 106, 104, 100) );
  cg.setColor( QColorGroup::Mid, QColor( 141, 138, 133) );
  cg.setColor( QColorGroup::Text, Qt::black );
  cg.setColor( QColorGroup::BrightText, Qt::white );
  cg.setColor( QColorGroup::ButtonText, Qt::black );
  cg.setColor( QColorGroup::Base, Qt::white );
  cg.setColor( QColorGroup::Background, ac_DisabledBackGroundColor );
  cg.setColor( QColorGroup::Shadow, Qt::black );
  cg.setColor( QColorGroup::Highlight, QColor( 0, 0, 128) );
  cg.setColor( QColorGroup::HighlightedText, Qt::white );
  cg.setColor( QColorGroup::Link, QColor( 0, 0, 255) );
  cg.setColor( QColorGroup::LinkVisited, QColor( 255, 0, 255) );
  pal->setInactive( cg );
  cg.setColor( QColorGroup::Foreground, QColor( 128, 128, 128) );
  cg.setColor( QColorGroup::Button, QColor( 212, 208, 200) );
  cg.setColor( QColorGroup::Light, Qt::white );
  cg.setColor( QColorGroup::Midlight, QColor( 243, 239, 230) );
  cg.setColor( QColorGroup::Dark, QColor( 106, 104, 100) );
  cg.setColor( QColorGroup::Mid, QColor( 141, 138, 133) );
  cg.setColor( QColorGroup::Text, QColor( 128, 128, 128) );
  cg.setColor( QColorGroup::BrightText, Qt::white );
  cg.setColor( QColorGroup::ButtonText, QColor( 128, 128, 128) );
  cg.setColor( QColorGroup::Base, Qt::white );
  cg.setColor( QColorGroup::Background, ac_DisabledBackGroundColor );
  cg.setColor( QColorGroup::Shadow, Qt::black );
  cg.setColor( QColorGroup::Highlight, QColor( 0, 0, 128) );
  cg.setColor( QColorGroup::HighlightedText, Qt::white );
  cg.setColor( QColorGroup::Link, QColor( 0, 0, 255) );
  cg.setColor( QColorGroup::LinkVisited, QColor( 255, 0, 255) );
  pal->setDisabled( cg );
#else
  pal->setColor( QPalette::Active, QPalette::Foreground, Qt::black );
  pal->setColor( QPalette::Active, QPalette::Button, ac_DisabledBackGroundColor );
  pal->setColor( QPalette::Active, QPalette::Light, Qt::white );
  pal->setColor( QPalette::Active, QPalette::Midlight, QColor( 233, 231, 227) );
  pal->setColor( QPalette::Active, QPalette::Dark, QColor( 106, 104, 100) );
  pal->setColor( QPalette::Active, QPalette::Mid, QColor( 141, 138, 133) );
  pal->setColor( QPalette::Active, QPalette::Text, Qt::black );
  pal->setColor( QPalette::Active, QPalette::BrightText, Qt::white );
  pal->setColor( QPalette::Active, QPalette::ButtonText, Qt::black );
  pal->setColor( QPalette::Active, QPalette::Base, Qt::white );
  pal->setColor( QPalette::Active, QPalette::Background, ac_DisabledBackGroundColor );
  pal->setColor( QPalette::Active, QPalette::Shadow, Qt::black );
  pal->setColor( QPalette::Active, QPalette::Highlight, QColor( 0, 0, 128) );
  pal->setColor( QPalette::Active, QPalette::HighlightedText, Qt::white );
  pal->setColor( QPalette::Active, QPalette::Link, Qt::black );
  pal->setColor( QPalette::Active, QPalette::LinkVisited, Qt::black );

  pal->setColor( QPalette::Inactive, QPalette::Foreground, Qt::black );
  pal->setColor( QPalette::Inactive, QPalette::Button, ac_DisabledBackGroundColor );
  pal->setColor( QPalette::Inactive, QPalette::Light, Qt::white );
  pal->setColor( QPalette::Inactive, QPalette::Midlight, QColor( 243, 239, 230) );
  pal->setColor( QPalette::Inactive, QPalette::Dark, QColor( 106, 104, 100) );
  pal->setColor( QPalette::Inactive, QPalette::Mid, QColor( 141, 138, 133) );
  pal->setColor( QPalette::Inactive, QPalette::Text, Qt::black );
  pal->setColor( QPalette::Inactive, QPalette::BrightText, Qt::white );
  pal->setColor( QPalette::Inactive, QPalette::ButtonText, Qt::black );
  pal->setColor( QPalette::Inactive, QPalette::Base, Qt::white );
  pal->setColor( QPalette::Inactive, QPalette::Background, ac_DisabledBackGroundColor );
  pal->setColor( QPalette::Inactive, QPalette::Shadow, Qt::black );
  pal->setColor( QPalette::Inactive, QPalette::Highlight, QColor( 0, 0, 128) );
  pal->setColor( QPalette::Inactive, QPalette::HighlightedText, Qt::white );
  pal->setColor( QPalette::Inactive, QPalette::Link, QColor( 0, 0, 255) );
  pal->setColor( QPalette::Inactive, QPalette::LinkVisited, QColor( 255, 0, 255) );

  pal->setColor( QPalette::Disabled, QPalette::Foreground, QColor( 128, 128, 128) );
  pal->setColor( QPalette::Disabled, QPalette::Button, ac_DisabledBackGroundColor );
  pal->setColor( QPalette::Disabled, QPalette::Light, Qt::white );
  pal->setColor( QPalette::Disabled, QPalette::Midlight, QColor( 243, 239, 230) );
  pal->setColor( QPalette::Disabled, QPalette::Dark, QColor( 106, 104, 100) );
  pal->setColor( QPalette::Disabled, QPalette::Mid, QColor( 141, 138, 133) );
  pal->setColor( QPalette::Disabled, QPalette::Text, QColor( 128, 128, 128) );
  pal->setColor( QPalette::Disabled, QPalette::BrightText, Qt::white );
  pal->setColor( QPalette::Disabled, QPalette::ButtonText, QColor( 128, 128, 128) );
  pal->setColor( QPalette::Disabled, QPalette::Base, Qt::white );
  pal->setColor( QPalette::Disabled, QPalette::Background, ac_DisabledBackGroundColor );
  pal->setColor( QPalette::Disabled, QPalette::Shadow, Qt::black );
  pal->setColor( QPalette::Disabled, QPalette::Highlight, QColor( 0, 0, 128) );
  pal->setColor( QPalette::Disabled, QPalette::HighlightedText, Qt::white );
  pal->setColor( QPalette::Disabled, QPalette::Link, QColor( 0, 0, 255) );
  pal->setColor( QPalette::Disabled, QPalette::LinkVisited, QColor( 255, 0, 255) );
#endif

  return pal;
}

QPalette* widgetattributes::gf_pInitEntirePaletteWithBGColor( const QColor& ac_DisabledBackGroundColor )
{
  QPalette* pal = new QPalette();
#if QT_VERSION < 0x040000
  QColorGroup cg;
  cg.setColor( QColorGroup::Foreground, Qt::black );
  cg.setColor( QColorGroup::Button, ac_DisabledBackGroundColor );
  cg.setColor( QColorGroup::Light, Qt::white );
  cg.setColor( QColorGroup::Midlight, QColor( 233, 231, 227) );
  cg.setColor( QColorGroup::Dark, QColor( 106, 104, 100) );
  cg.setColor( QColorGroup::Mid, QColor( 141, 138, 133) );
  cg.setColor( QColorGroup::Text, Qt::black );
  cg.setColor( QColorGroup::BrightText, Qt::white );
  cg.setColor( QColorGroup::ButtonText, Qt::black );
  cg.setColor( QColorGroup::Base, Qt::white );
  cg.setColor( QColorGroup::Background, ac_DisabledBackGroundColor );
  cg.setColor( QColorGroup::Shadow, Qt::black );
  cg.setColor( QColorGroup::Highlight, QColor( 0, 0, 128) );
  cg.setColor( QColorGroup::HighlightedText, Qt::white );
  cg.setColor( QColorGroup::Link, Qt::black );
  cg.setColor( QColorGroup::LinkVisited, Qt::black );
  pal->setActive( cg );
  cg.setColor( QColorGroup::Foreground, Qt::black );
  cg.setColor( QColorGroup::Button, ac_DisabledBackGroundColor );
  cg.setColor( QColorGroup::Light, Qt::white );
  cg.setColor( QColorGroup::Midlight, QColor( 243, 239, 230) );
  cg.setColor( QColorGroup::Dark, QColor( 106, 104, 100) );
  cg.setColor( QColorGroup::Mid, QColor( 141, 138, 133) );
  cg.setColor( QColorGroup::Text, Qt::black );
  cg.setColor( QColorGroup::BrightText, Qt::white );
  cg.setColor( QColorGroup::ButtonText, Qt::black );
  cg.setColor( QColorGroup::Base, Qt::white );
  cg.setColor( QColorGroup::Background, ac_DisabledBackGroundColor );
  cg.setColor( QColorGroup::Shadow, Qt::black );
  cg.setColor( QColorGroup::Highlight, QColor( 0, 0, 128) );
  cg.setColor( QColorGroup::HighlightedText, Qt::white );
  cg.setColor( QColorGroup::Link, QColor( 0, 0, 255) );
  cg.setColor( QColorGroup::LinkVisited, QColor( 255, 0, 255) );
  pal->setInactive( cg );
  cg.setColor( QColorGroup::Foreground, QColor( 128, 128, 128) );
  cg.setColor( QColorGroup::Button, ac_DisabledBackGroundColor );
  cg.setColor( QColorGroup::Light, Qt::white );
  cg.setColor( QColorGroup::Midlight, QColor( 243, 239, 230) );
  cg.setColor( QColorGroup::Dark, QColor( 106, 104, 100) );
  cg.setColor( QColorGroup::Mid, QColor( 141, 138, 133) );
  cg.setColor( QColorGroup::Text, QColor( 128, 128, 128) );
  cg.setColor( QColorGroup::BrightText, Qt::white );
  cg.setColor( QColorGroup::ButtonText, QColor( 128, 128, 128) );
  cg.setColor( QColorGroup::Base, Qt::white );
  cg.setColor( QColorGroup::Background, ac_DisabledBackGroundColor );
  cg.setColor( QColorGroup::Shadow, Qt::black );
  cg.setColor( QColorGroup::Highlight, QColor( 0, 0, 128) );
  cg.setColor( QColorGroup::HighlightedText, Qt::white );
  cg.setColor( QColorGroup::Link, QColor( 0, 0, 255) );
  cg.setColor( QColorGroup::LinkVisited, QColor( 255, 0, 255) );
  pal->setDisabled( cg );
#else
  pal->setColor( QPalette::Active, QPalette::Foreground, Qt::black );
  pal->setColor( QPalette::Active, QPalette::Button, ac_DisabledBackGroundColor );
  pal->setColor( QPalette::Active, QPalette::Light, Qt::white );
  pal->setColor( QPalette::Active, QPalette::Midlight, QColor( 233, 231, 227) );
  pal->setColor( QPalette::Active, QPalette::Dark, QColor( 106, 104, 100) );
  pal->setColor( QPalette::Active, QPalette::Mid, QColor( 141, 138, 133) );
  pal->setColor( QPalette::Active, QPalette::Text, Qt::black );
  pal->setColor( QPalette::Active, QPalette::BrightText, Qt::white );
  pal->setColor( QPalette::Active, QPalette::ButtonText, Qt::black );
  pal->setColor( QPalette::Active, QPalette::Base, Qt::white );
  pal->setColor( QPalette::Active, QPalette::Background, ac_DisabledBackGroundColor );
  pal->setColor( QPalette::Active, QPalette::Shadow, Qt::black );
  pal->setColor( QPalette::Active, QPalette::Highlight, QColor( 0, 0, 128) );
  pal->setColor( QPalette::Active, QPalette::HighlightedText, Qt::white );
  pal->setColor( QPalette::Active, QPalette::Link, Qt::black );
  pal->setColor( QPalette::Active, QPalette::LinkVisited, Qt::black );

  pal->setColor( QPalette::Inactive, QPalette::Foreground, Qt::black );
  pal->setColor( QPalette::Inactive, QPalette::Button, ac_DisabledBackGroundColor );
  pal->setColor( QPalette::Inactive, QPalette::Light, Qt::white );
  pal->setColor( QPalette::Inactive, QPalette::Midlight, QColor( 243, 239, 230) );
  pal->setColor( QPalette::Inactive, QPalette::Dark, QColor( 106, 104, 100) );
  pal->setColor( QPalette::Inactive, QPalette::Mid, QColor( 141, 138, 133) );
  pal->setColor( QPalette::Inactive, QPalette::Text, Qt::black );
  pal->setColor( QPalette::Inactive, QPalette::BrightText, Qt::white );
  pal->setColor( QPalette::Inactive, QPalette::ButtonText, Qt::black );
  pal->setColor( QPalette::Inactive, QPalette::Base, Qt::white );
  pal->setColor( QPalette::Inactive, QPalette::Background, ac_DisabledBackGroundColor );
  pal->setColor( QPalette::Inactive, QPalette::Shadow, Qt::black );
  pal->setColor( QPalette::Inactive, QPalette::Highlight, QColor( 0, 0, 128) );
  pal->setColor( QPalette::Inactive, QPalette::HighlightedText, Qt::white );
  pal->setColor( QPalette::Inactive, QPalette::Link, QColor( 0, 0, 255) );
  pal->setColor( QPalette::Inactive, QPalette::LinkVisited, QColor( 255, 0, 255) );

  pal->setColor( QPalette::Disabled, QPalette::Foreground, QColor( 128, 128, 128) );
  pal->setColor( QPalette::Disabled, QPalette::Button, ac_DisabledBackGroundColor );
  pal->setColor( QPalette::Disabled, QPalette::Light, Qt::white );
  pal->setColor( QPalette::Disabled, QPalette::Midlight, QColor( 243, 239, 230) );
  pal->setColor( QPalette::Disabled, QPalette::Dark, QColor( 106, 104, 100) );
  pal->setColor( QPalette::Disabled, QPalette::Mid, QColor( 141, 138, 133) );
  pal->setColor( QPalette::Disabled, QPalette::Text, QColor( 128, 128, 128) );
  pal->setColor( QPalette::Disabled, QPalette::BrightText, Qt::white );
  pal->setColor( QPalette::Disabled, QPalette::ButtonText, QColor( 128, 128, 128) );
  pal->setColor( QPalette::Disabled, QPalette::Base, Qt::white );
  pal->setColor( QPalette::Disabled, QPalette::Background, ac_DisabledBackGroundColor );
  pal->setColor( QPalette::Disabled, QPalette::Shadow, Qt::black );
  pal->setColor( QPalette::Disabled, QPalette::Highlight, QColor( 0, 0, 128) );
  pal->setColor( QPalette::Disabled, QPalette::HighlightedText, Qt::white );
  pal->setColor( QPalette::Disabled, QPalette::Link, QColor( 0, 0, 255) );
  pal->setColor( QPalette::Disabled, QPalette::LinkVisited, QColor( 255, 0, 255) );
#endif

  return pal;
}


  //!labview style naming
void widgetattributes::gf_Indicator( QWidget* a_pVictim, const QPalette* ac_pPalette )
{
  if( ac_pPalette )
    a_pVictim->setPalette( *ac_pPalette );
  else if( sc_pPalette )
    a_pVictim->setPalette( *sc_pPalette );
  a_pVictim->setEnabled( false );
}

void widgetattributes::gf_Control( QWidget* a_pVictim, const QPalette* ac_pPalette )
{
  if( ac_pPalette )
    a_pVictim->setPalette( *ac_pPalette );
  a_pVictim->setEnabled( true );
}
