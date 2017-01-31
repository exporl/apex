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
 
#include "labeledit.h"
#include "customlayout.h"

#if QT_VERSION < 0x040000
#include <qlabel.h>
#include <qpushbutton.h>
#else
#include <QLabel>
#include <QPushButton>
#endif

using namespace guicore;

LabeldEdit::LabeldEdit(QWidget* a_pParent, const QString& ac_sText, const FriendLayout::mt_eFriendPosition ac_eWhere,
                           const double ac_dLabelSize ) :
    LineEdit( a_pParent, ac_sText ),
  m_pLabel( new QLabel( ac_sText, a_pParent ) ),
  mc_pLayout( new FriendLayout( (QWidget*) this, m_pLabel, ac_eWhere, ac_dLabelSize ) )
{
  switch( ac_eWhere )
  {
#if QT_VERSION < 0x040000
    case FriendLayout::right: m_pLabel->setAlignment( QLabel::AlignRight ); break;
    case FriendLayout::top: m_pLabel->setAlignment( QLabel::AlignCenter ); break;
    case FriendLayout::bottom: m_pLabel->setAlignment( QLabel::AlignCenter ); break;
#else
    case FriendLayout::right: m_pLabel->setAlignment( Qt::AlignRight ); break;
    case FriendLayout::top: m_pLabel->setAlignment( Qt::AlignCenter ); break;
    case FriendLayout::bottom: m_pLabel->setAlignment( Qt::AlignCenter ); break;
#endif
    default : break;
  }
}
LabeldEdit::~LabeldEdit()
{
  delete mc_pLayout;
}

#if QT_VERSION < 0x040000
void LabeldEdit::mp_SetLabelAlignment( int a_Align )
#else
void LabeldEdit::mp_SetLabelAlignment( Qt::Alignment a_Align )
#endif
{
  m_pLabel->setAlignment( a_Align );
}

void LabeldEdit::resizeEvent( QResizeEvent* )
{
  /*QRect curR( QLineEdit::contentsRect() );
  QRect newR( curR.left(), curR.top(), e->size().width(), e->size().height() );
  guicore::GridLayout theL( 1, 2 );
  theL.mp_AddItem( (QWidget*)this, 0, 0 );
  theL.mp_AddItem( m_pLabel, 0, 1 );
  theL.mp_ResizeThemAll( newR );*/
}

void LabeldEdit::setGeometry( const QRect& ac_Rect )
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
