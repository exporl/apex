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

#include "flashplayerwidget.h"
#include "apextools.h"

#include <QResizeEvent>
#include <QColor>

#ifdef WIN32

#include "flashwnd.h"

FlashPlayerWidget::FlashPlayerWidget( QWidget* a_pParent ) :
    QPushButton( a_pParent ),
  m_pMovie( new CFlashWnd() ),
  mv_bEmit( false ),
  mv_bCreated( false )  //no idea how to find out if the window is initted so let's keep our own flag
{
}

FlashPlayerWidget::~FlashPlayerWidget()
{
  delete m_pMovie;
}

bool FlashPlayerWidget::mf_bCreateFlashObject()
{
  return mv_bCreated = m_pMovie->Create( ShockwaveFlashObjects::CLSID_ShockwaveFlash, 0,
                                         WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, this->winId(), 0 );
}

void FlashPlayerWidget::resizeEvent( QResizeEvent * e )
{
  const QSize cur( e->size() );
  ::SetWindowPos( m_pMovie->GetHWND(), HWND_TOP, 0, 0, cur.width(), cur.height(), 0 );
  mp_AdjustFlashColour();
  mp_GetFlash()->PutScaleMode( 0 );
}

bool FlashPlayerWidget::winEvent( MSG* msg, long* result )
{
  if( mv_bEmit &&
      msg->message == WM_MOUSEACTIVATE &&
      msg->lParam == 33619969L )  //only react on left click
  {
    emit ms_FlashClicked();
    *result = MA_NOACTIVATE;
    return true;  //disallow further processing
  }
  return false;
}

void FlashPlayerWidget::mp_Start()
{
  if( mv_bCreated )
    mp_GetFlash()->Play();
  else
    qDebug( "FlashPlayerWidget: create me first!" );
}

void FlashPlayerWidget::mp_Stop()
{
  if( mv_bCreated )
    mp_GetFlash()->Stop();
}

void FlashPlayerWidget::mp_GoToFrame( const long ac_nFrameNum )
{
  if( mv_bCreated )
    mp_GetFlash()->GotoFrame( ac_nFrameNum );
}

bool FlashPlayerWidget::mf_bFinished() const
{
  return mv_bCreated ? ( !mp_GetFlash()->IsPlaying() ) : false;
}

bool FlashPlayerWidget::mp_bSetMovie( const QString& ac_sPath )
{
  if( !mv_bCreated )
  {
    qDebug( "FlashPlayerWidget: create me first!" );
    return false;
  }

  // flash wants the absolute path
  QString path( apex::ApexTools::absolutePath(ac_sPath));

  mp_GetFlash()->PutMovie( path.ascii() );
  mp_AdjustFlashColour();
  mp_GetFlash()->PutScaleMode( 0 );
  mp_GetFlash()->Stop();
  mp_GetFlash()->GotoFrame( 0L );
  long a = mp_GetFlash()->GetFrameNum();
  if( a == -1L )
    return false;
  return true;
}

double FlashPlayerWidget::GetAspectRatio()
{
  return 1.0;
}

ShockwaveFlashObjects::IShockwaveFlash* FlashPlayerWidget::mp_GetFlash() const
{
  return m_pMovie->m_lpControl;
}

void FlashPlayerWidget::mp_AdjustFlashColour()
{
  if( mv_bCreated )
  {
    QColor color( palette().color( QPalette::Active, QPalette::Window ) );
    mp_GetFlash()->PutBackgroundColor( color.red()*0x10000 + color.green()*0x100 + color.blue() );
  }
}

#endif
