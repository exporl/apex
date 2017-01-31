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
 
#include "simpledialogbox.h"

#if QT_VERSION < 0x040000
#include <qpixmap.h>
#else
#include <QPixmap>
#endif

using namespace guicore;

SimpleDialogBox::SimpleDialogBox( const QMessageBox::Icon ac_eIcon,
                                  const QString& ac_sCaption,
                                  const QString& ac_sErrorMessage,
                                  const QString& ac_sButton1,
                                  const QString& ac_sButton2,
                                  const QString& ac_sButton3 ) :
  m_pMb( 0 )
{
  int nButtons = 1;
  if( ! ac_sButton2.isEmpty() )
    ++nButtons;
  if( ! ac_sButton3.isEmpty() )
    ++nButtons;

  int button1 = QMessageBox::Ok;
  int button2 = QMessageBox::NoButton;
  int button3 = QMessageBox::NoButton;

  if( nButtons > 1 )
    button2 = QMessageBox::Yes;     //cannot be Ok or we cannot set text later
  if( nButtons > 2 )
    button3 = QMessageBox::No;      //has to be different for same reason as above

  m_pMb = new QMessageBox( ac_sCaption, ac_sErrorMessage, ac_eIcon,
                           button1 | QMessageBox::Default,
                           button2,
                           button3 );

  if( ! ac_sButton1.isEmpty() )
    m_pMb->setButtonText( button1, ac_sButton1 );
  if( ! ac_sButton2.isEmpty() )
    m_pMb->setButtonText( button2, ac_sButton2 );
  if( ! ac_sButton3.isEmpty() )
    m_pMb->setButtonText( button3, ac_sButton3 );
}

SimpleDialogBox::~SimpleDialogBox()
{
  delete m_pMb;
}

const int SimpleDialogBox::exec()
{
  return m_pMb->exec();
}

void SimpleDialogBox::sf_MessageBox( const QString& ac_sMessage, const SimpleDialogBox::mt_eType ac_eType )
{
  if( ac_eType == mc_eError )
  {
    QMessageBox box( "Error.", ac_sMessage, QMessageBox::Critical, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );
    box.exec();
  }
  else if( ac_eType == mc_eWarning )
  {
    QMessageBox box( "Warning.", ac_sMessage, QMessageBox::Warning, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );
    box.exec();
  }
  else
  {
    QMessageBox box( "Information.", ac_sMessage, QMessageBox::Information, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );
    box.exec();
  }
}

void SimpleDialogBox::sf_MessageBox( const QString& ac_sCaption, const QString& ac_sMessage, const mt_eType ac_eType )
{
  if( ac_eType == mc_eError )
  {
    QMessageBox box( ac_sCaption, ac_sMessage, QMessageBox::Critical, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );
    box.exec();
  }
  else if( ac_eType == mc_eWarning )
  {
    QMessageBox box( ac_sCaption, ac_sMessage, QMessageBox::Warning, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );
    box.exec();
  }
  else
  {
    QMessageBox box( ac_sCaption, ac_sMessage, QMessageBox::Information, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );
    box.exec();
  }
}

void SimpleDialogBox::sf_MessageBox( const QString& ac_sIcon, const QString& ac_sCaption, const QString& ac_sMessage, const mt_eType ac_eType )
{
  if( ac_eType == mc_eError )
  {
    QMessageBox box( ac_sCaption, ac_sMessage, QMessageBox::Critical, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );
#if QT_VERSION < 0x040000
    box.setIcon( QPixmap( ac_sIcon ) );
#else
    QImage im( ac_sIcon );
    box.setIconPixmap( QPixmap::fromImage( im ) );
#endif
    box.exec();
  }
  else if( ac_eType == mc_eWarning )
  {
    QMessageBox box( ac_sCaption, ac_sMessage, QMessageBox::Warning, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );
#if QT_VERSION < 0x040000
    box.setIcon( QPixmap( ac_sIcon ) );
#else
    QImage im( ac_sIcon );
    box.setIconPixmap( QPixmap::fromImage( im ) );
#endif
    box.exec();
  }
  else
  {
    QMessageBox box( ac_sCaption, ac_sMessage, QMessageBox::Information, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );
#if QT_VERSION < 0x040000
    box.setIcon( QPixmap( ac_sIcon ) );
#else
    QImage im( ac_sIcon );
    box.setIconPixmap( QPixmap::fromImage( im ) );
#endif
    box.exec();
  }
}

void SimpleDialogBox::sf_MessageBox( const QPixmap& ac_sIcon, const QString& ac_sCaption, const QString& ac_sMessage, const mt_eType ac_eType )
{
  if( ac_eType == mc_eError )
  {
    QMessageBox box( ac_sCaption, ac_sMessage, QMessageBox::Critical, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );
#if QT_VERSION < 0x040000
    box.setIcon( ac_sIcon );
#else
    box.setIconPixmap( ac_sIcon );
#endif
    box.exec();
  }
  else if( ac_eType == mc_eWarning )
  {
    QMessageBox box( ac_sCaption, ac_sMessage, QMessageBox::Warning, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );
#if QT_VERSION < 0x040000
    box.setIcon( ac_sIcon );
#else
    box.setIconPixmap( ac_sIcon);
#endif
    box.exec();
  }
  else
  {
    QMessageBox box( ac_sCaption, ac_sMessage, QMessageBox::Information, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );
#if QT_VERSION < 0x040000
    box.setIcon( ac_sIcon );
#else
    box.setIconPixmap( ac_sIcon );
#endif
    box.exec();
  }
}
