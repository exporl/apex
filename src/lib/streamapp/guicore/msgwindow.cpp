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
 
#include "msgwindow.h"
#include "appcore/qt_utils.h"
#include "simpledialogbox.h"
#include "widgetattributes.h"

#if QT_VERSION < 0x040000
#include <qtextedit.h>
#include <qclipboard.h>
#include <qpushbutton.h>
#include <qfiledialog.h>
#include <qapplication.h>
#else
#include <QTextEdit>
#include <QClipboard>
#include <QPushButton>
#include <QFileDialog>
#include <QApplication>
#include <QTextStream>
#endif

using namespace guicore;
using namespace streamapp;

namespace
{
  const QColor sc_Message( 0, 0, 0 );
  const QColor sc_MessageInv( 240, 240, 240 );
  const QColor sc_Warning( 34, 208, 38 );
  const QColor sc_Error( 255, 0, 0 );
}

MessageWindow::MessageWindow( QWidget* a_pParent ) :
    CustomLayoutWidget<>( a_pParent ),
  mv_bTimeStamp( false ),
  mv_bAllowClose( false ),
  mv_bInvertColor( false )
{
  m_pMess = new QTextEdit( this );
  m_pMess->setReadOnly( true );

  m_pCopy = new QPushButton( "Copy to Clipboard", this );
  m_pSave = new QPushButton( "Save to File", this );

  m_pLayout = new CustomLayout();
  m_pLayout->mp_AddItem( m_pMess, new tItemLayout( 0.0, 0.0, 1.0, 0.85 ) );
  tItemLayout* pC = new tItemLayout( 0.1, 0.85, 0.3, 0.15, 1.0, 0.8 );
  //pC->m_bPhiRatio = true;
  m_pLayout->mp_AddItem( m_pCopy, pC );
  m_pLayout->mp_AddItem( m_pSave, new tItemLayout( 0.6, 0.85, 0.3, 0.15, 1.0, 0.8 ) );

  connect( m_pCopy, SIGNAL( released() ), SLOT( ms_Copy() ) );
  connect( m_pSave, SIGNAL( released() ), SLOT( ms_Save() ) );
}

MessageWindow::~MessageWindow()
{
  if( !m_sAutoSave.isNull() )
    mp_Save( m_sAutoSave );
}

void MessageWindow::ms_Message( const QString& ac_sMess )
{
  ms_Message( ac_sMess, gc_eMessage );
}

void MessageWindow::ms_Message( const QString& ac_sMess, const gt_eMsgType& ac_eType )
{
    //set color
  if( ac_eType == gc_eMessage )
  {
    if( mv_bInvertColor )
#if QT_VERSION < 0x040000
      m_pMess->setColor( sc_MessageInv );
#else
      m_pMess->setTextColor( sc_MessageInv );
#endif
    else
#if QT_VERSION < 0x040000
      m_pMess->setColor( sc_Message );
#else
      m_pMess->setTextColor( sc_Message );
#endif
  }
  else if( ac_eType == gc_eWarning )
#if QT_VERSION < 0x040000
    m_pMess->setColor( sc_Warning );
#else
    m_pMess->setTextColor( sc_Warning );
#endif
  else
#if QT_VERSION < 0x040000
    m_pMess->setColor( sc_Error );
#else
    m_pMess->setTextColor( sc_Error );
#endif

    //add message
  if( mv_bTimeStamp )
    m_pMess->append( f_sTimeStringSeconds() + " " + ac_sMess );
  else
    m_pMess->append( ac_sMess );
}

void MessageWindow::mp_SetFont( const QFont& ac_Font )
{
  m_pMess->setFont( ac_Font );
}

QFont MessageWindow::mf_GetFont() const
{
  return m_pMess->font();
}

void MessageWindow::mp_SetNegativeColor( const bool ac_bSet )
{
  if( ac_bSet )
    widgetattributes::gf_SetBackGroundColor( m_pMess, Qt::black );
  else
    widgetattributes::gf_SetBackGroundColor( m_pMess, Qt::white );

#if QT_VERSION > 0x040000
  setAutoFillBackground( true ); //FIXME this doesn't do ANYTHING
#endif

  mv_bInvertColor = ac_bSet;
}

void MessageWindow::ms_Clear()
{
  m_pMess->clear();
}

void MessageWindow::ms_Copy()
{
  QClipboard* cb = QApplication::clipboard();
#if QT_VERSION < 0x040000
  cb->setText( m_pMess->text() );
#else
  cb->setText( m_pMess->toPlainText() );
#endif
}

void MessageWindow::ms_Save()
{
#if QT_VERSION < 0x040000
  const QString sFile = QFileDialog::getSaveFileName( "./" );
#else
  const QString sFile = QFileDialog::getSaveFileName( this, "./" );
#endif
  if( !sFile.isEmpty() )
    mp_Save( sFile );
}

void MessageWindow::mp_Save( const QString& ac_sFile )
{
  QFile file( ac_sFile );
#if QT_VERSION < 0x040000
  if( file.open( IO_WriteOnly ) )
#else
  if( file.open( QIODevice::WriteOnly ) )
#endif
  {
    QTextStream s( &file );
#if QT_VERSION < 0x040000
    s << m_pMess->text();
#else
    s << m_pMess->toPlainText();
#endif
    file.close();
  }
  else
  {
    SimpleDialogBox::sf_MessageBox( "Error.", "Couldn't open file for writing." );
  }
}

void MessageWindow::closeEvent( QCloseEvent* e )
{
  if( mv_bAllowClose )
    CustomLayoutWidget<>::closeEvent( e );
}
