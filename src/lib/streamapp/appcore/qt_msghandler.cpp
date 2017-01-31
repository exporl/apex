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

#include "qt_msghandler.h"
#include "callback/callback.h"
#include <iostream>

using namespace appcore;
using namespace streamapp;

appcore::QtMsgHandler::QtMsgHandler()
{
  qInstallMsgHandler( sf_MessageOutput );
}

appcore::QtMsgHandler::~QtMsgHandler()
{
  qInstallMsgHandler( 0 );
}

Callback* appcore::QtMsgHandler::sm_pFatalCb = 0;

void appcore::QtMsgHandler::mp_InstallFatalCallback( streamapp::Callback* a_pCallback )
{
  sm_pFatalCb = a_pCallback;
}

void appcore::QtMsgHandler::sf_MessageOutput( QtMsgType a_Type, const char* ac_psMsg )
{
  static char sBuf[ 200 ];
  switch( a_Type )
  {
    case QtDebugMsg:
      sprintf( sBuf, "qDebug: %s\n", ac_psMsg );
      std::cout << sBuf;
      break;
    case QtWarningMsg:
      sprintf( sBuf, "qWarning: %s\n", ac_psMsg );
      break;
#if QT_VERSION > 0x040000
    case QtCriticalMsg:
      sprintf( sBuf, "qCritical: %s\n", ac_psMsg );
      std::cout << sBuf;
      break;
#endif
    case QtFatalMsg:
      sprintf( sBuf, "qFatal: %s\n", ac_psMsg );
      std::cout << sBuf;
      if( sm_pFatalCb )
        sm_pFatalCb->mf_Callback();
  }
}

namespace
{

    /**
      * Callback for std::out.
      */
  void outcallback( const char *ptr, std::streamsize count )
  {
    if( *ptr != '\n' )  //ignore eof
    {
      if( count > 1 )
        QtMsgCatcher::sf_pInstance()->ms_DoMessage( QString( ptr ), gc_eMessage );
      else
        QtMsgCatcher::sf_pInstance()->ms_DoMessage( QString( QChar( *ptr ) ), gc_eMessage );
    }
  }

    /**
      * Callback for std::err.
      */
  void errcallback( const char *ptr, std::streamsize count )
  {
    if( *ptr != '\n' )
    {
      if( count > 1 )
        QtMsgCatcher::sf_pInstance()->ms_DoMessage( QString( ptr ), gc_eError );
      else
        QtMsgCatcher::sf_pInstance()->ms_DoMessage( QString( QChar( *ptr ) ), gc_eError );
    }
  }

}

QtMsgCatcher::QtMsgCatcher() :
  mv_bEmitType( true ),
  m_OutRedir( std::cout, outcallback ),
  m_ErrRedir( std::cerr, errcallback )
{
  qInstallMsgHandler( sf_QtMsgHandler );
}

QtMsgCatcher::~QtMsgCatcher()
{
  qInstallMsgHandler( 0 );
}

void QtMsgCatcher::ms_DoMessage( const QString& ac_sMess, const streamapp::gt_eMsgType& ac_eType )
{
  if( mv_bEmitType )
    emit ms_Message( ac_sMess, ac_eType );
  else
    emit ms_Message( ac_sMess );
}

void QtMsgCatcher::sf_QtMsgHandler( QtMsgType a_Type, const char* ac_psMsg )
{
  static char sBuf[ 200 ];
  switch( a_Type )
  {
    case QtDebugMsg:
      sprintf( sBuf, "qDebug: %s\n", ac_psMsg );
      QtMsgCatcher::sf_pInstance()->ms_DoMessage( QString( sBuf ), gc_eMessage );
      break;
    case QtWarningMsg:
      sprintf( sBuf, "qWarning: %s\n", ac_psMsg );
      QtMsgCatcher::sf_pInstance()->ms_DoMessage( QString( sBuf ), gc_eWarning );
      break;
#if QT_VERSION > 0x040000
    case QtCriticalMsg:
      sprintf( sBuf, "qCritical: %s\n", ac_psMsg );
      QtMsgCatcher::sf_pInstance()->ms_DoMessage( QString( sBuf ), gc_eCritical );
      break;
#endif
    case QtFatalMsg:
      sprintf( sBuf, "qFatal: %s\n", ac_psMsg );
      QtMsgCatcher::sf_pInstance()->ms_DoMessage( QString( sBuf ), gc_eFatal );
      break;
  }

}
