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
 
#include "userinputbase.h"
#include "guicore/customlayout.h"

#if QT_VERSION < 0x040000
#include <qpushbutton.h>
#else
#include <QPushButton>
#endif

using namespace guicore;

UserInputBase::UserInputBase( const bool ac_bUseRetry, QWidget* parent )
  : QDialog( parent )
{
  setSizeGripEnabled( TRUE );
  buttonOk = new QPushButton( this );
  buttonCancel = new QPushButton( this );

  buttonOk->setDefault( TRUE );
  buttonOk->setAutoDefault( TRUE );
  buttonOk->setText( tr( "&OK" ) );
  buttonCancel->setText( tr( "&Cancel" ) );

  layout = new CustomLayout();

  tItemLayout* pOk = new tItemLayout();
  pOk->m_dXoff = 0.1;
  pOk->m_dYoff = 0.7;
  pOk->m_dXsiz = 0.2;
  pOk->m_dYsiz = 0.2;
  layout->mp_AddItem( buttonOk, pOk );

  tItemLayout* pCancel = new tItemLayout();
  pCancel->m_dXoff = 0.7;
  pCancel->m_dYoff = 0.7;
  pCancel->m_dXsiz = 0.2;
  pCancel->m_dYsiz = 0.2;
  layout->mp_AddItem( buttonCancel, pCancel );

  if( ac_bUseRetry )
  {
    buttonRetry = new QPushButton( this );
    buttonRetry->setText( "&Retry" );
    tItemLayout* pretry = new tItemLayout();
    pretry->m_dXoff = 0.4;
    pretry->m_dYoff = 0.7;
    pretry->m_dXsiz = 0.2;
    pretry->m_dYsiz = 0.2;
    layout->mp_AddItem( buttonRetry, pretry );
    connect( buttonRetry, SIGNAL( clicked() ), this, SLOT( ms_Retry() ) );
  }

  resize( QSize(436, 142).expandedTo(minimumSizeHint()) );

  // signals and slots connections
  connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

UserInputBase::~UserInputBase()
{
  delete layout;
}

void UserInputBase::mp_SetNoCancel( const bool ac_bSet )
{
  if( ac_bSet )
  {
    tItemLayout* pL = layout->mf_pGetLayoutFor( buttonOk );
    pL->m_dXoff = 0.4;
    buttonCancel->hide();
  }
  else
  {
    tItemLayout* pL = layout->mf_pGetLayoutFor( buttonOk );
    pL->m_dXoff = 0.1;
    buttonCancel->show();
  }
}

void UserInputBase::resizeEvent( QResizeEvent* )
{
  layout->mp_ResizeThemAll( QDialog::rect() );
}

void UserInputBase::ms_Retry()
{
  done( 3 );
}
