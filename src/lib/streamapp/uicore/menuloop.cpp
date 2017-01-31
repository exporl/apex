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
 
#include "menuloop.h"
#include "menu.h"
#include "screen.h"
#include "controller/buttoncontroller.h"
#include "appcore/threads/thread.h"
#ifdef S_DEBUG
#include "utils/tracer.h"
#endif
#ifdef USESQT
#if QT_VERSION < 0x040000
#include <qeventloop.h>
#include <qapplication.h>
#else
#include <QEventLoop>
#include <QApplication>
#endif
#endif

using namespace uicore;

MenuLoop::MenuLoop( Menu* a_pMenu, IButtonController* a_pControl, Screen* a_pScreen ) :
  m_pMenu( a_pMenu ),
  m_pScreen( a_pScreen ),
  m_pCtrl( a_pControl ),
  mv_bQuit( false ),
  mv_nPeriod( 100 )
{
}

MenuLoop::~MenuLoop()
{
  mp_Stop();
  IThread::sf_Sleep( mv_nPeriod );  //...
}

void MenuLoop::mp_SetPeriod( const unsigned ac_nMilliSeconds )
{
  const Lock L( mc_Lock );
  mv_nPeriod = ac_nMilliSeconds;
}

void MenuLoop::mp_Stop()
{
  const Lock L( mc_Lock );
  mv_bQuit = true;
}

bool MenuLoop::mf_bQuit()
{
  const Lock L( mc_Lock );
  return mv_bQuit;
}

void MenuLoop::mf_Exec()
{
  IButtonController::mt_eControlCode code = IButtonController::mc_eNoButton;
  while( !mf_bQuit() )
  {
    m_pCtrl->mf_bPoll( code );
    switch( code )
    {
      case IButtonController::mc_ePower :
      {
        TRACE( "MenuLoop: exiting" );
        mp_Stop();
        break;
      }
      case IButtonController::mc_eUp :
        m_pMenu->mf_Up(); break;
      case IButtonController::mc_eDown :
        m_pMenu->mf_Down(); break;
      case IButtonController::mc_eLeft :
        m_pMenu->mf_Left(); break;
      case IButtonController::mc_eRight :
        m_pMenu->mf_Right(); break;
      case IButtonController::mc_eSelect :
        m_pMenu->mf_Select(); break;
      default:
        break;
    };
    if( code != IButtonController::mc_eNoButton )
    {
      m_pScreen->mf_DrawNoErase();
      DBGPF( "got code %#x", code );
    }
#ifdef USESQT
  #if QT_VERSION < 0x040000
    QApplication::eventLoop()->processEvents( QEventLoop::AllEvents, mv_nPeriod );
  #else
    QApplication::processEvents( QEventLoop::AllEvents, mv_nPeriod );
  #endif
#endif
    IThread::sf_Sleep( mv_nPeriod );
  }
}
