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
 
#include "fileselector.h"
#include "customlayout.h"

#if QT_VERSION < 0x040000
#include <qpushbutton.h>
#include <qfiledialog.h>
#else
#include <QPushButton>
#include <QFileDialog>
#endif

using namespace guicore;

FileSelector::FileSelector( QWidget* a_pParent, const guicore::FriendLayout::mt_eFriendPosition ac_eWhere ) :
    LineEdit( a_pParent, "" ),
  mc_pButton( new QPushButton( "..", a_pParent ) ),
  mc_pLayout( new FriendLayout( mc_pButton, (QWidget*) this, ac_eWhere, 0.9 ) ),
  mv_sStartDir( "./" ),
  mv_sSelected(),
  mv_sSelect( "All Files (*.*)" ),
  mv_eMode( mc_eExistingFile )
{
  mc_pButton->show();
  connect( mc_pButton, SIGNAL( released() ), SLOT( mp_Select() ) );
}

FileSelector::~FileSelector()
{
  delete mc_pLayout;
}

void FileSelector::setGeometry( const QRect &ac_Rect )
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

void FileSelector::resizeEvent( QResizeEvent* /*e*/ )
{
  //??
}

void FileSelector::mp_Select()
{
  switch( mv_eMode )
  {
#if QT_VERSION < 0x040000
    case mc_eExistingFile : mv_sSelected = QFileDialog::getOpenFileName( mv_sStartDir, mv_sSelect, 0, 0, mv_sTitle ); break;
    case mc_eNewFile      : mv_sSelected = QFileDialog::getSaveFileName( mv_sStartDir, mv_sSelect, 0, 0, mv_sTitle ); break;
    case mc_eExistingDir  : mv_sSelected = QFileDialog::getExistingDirectory( mv_sStartDir ); break;
#else
    case mc_eExistingFile : mv_sSelected = QFileDialog::getOpenFileName( 0, mv_sStartDir, mv_sSelect ); break;
    case mc_eNewFile      : mv_sSelected = QFileDialog::getSaveFileName( 0, mv_sStartDir, mv_sSelect ); break;
    case mc_eExistingDir  : mv_sSelected = QFileDialog::getExistingDirectory( 0, mv_sStartDir ); break;
#endif
    default: break;
  }

  if( !mv_sSelected.isEmpty() )
  {
    QLineEdit::setText( mv_sSelected );
    emit( ms_Selected( mv_sSelected ) );
  }
}
