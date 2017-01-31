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
 
#ifndef __QT_MESSAGEREDIRECTOR_H__
#define __QT_MESSAGEREDIRECTOR_H__

#include "typedefs.h"
#include "singleton.h"
#include "utils/stdredirector.h"

#if QT_VERSION < 0x040000
#include <qglobal.h>
#include <qobject.h>
#else
#include <QObject>
#include <QGlobalStatic>
#endif

namespace streamapp
{
  class Callback;
}

namespace appcore
{
    /**
      * QtMsgHandler
      *   redirects all Qt messages to std::cout.
      *   Usage: use once early in the program, eg in in main() :
      *
      *   @code
      *   QtMsgHandler::sf_pInstance();
      *   @endcode
      *
      *   This makes all qDebug, qWarning etc to be printed to std::cout,
      *   prefixed with the one they came from (eg qDebug( "o" ) becomes
      *   std::cout << "qDebug: " << "o" << std::endl;)
      *   Note: when using qFatal on windows it seems to crash
      *   the application.
      ******************************************************************* */
  class QtMsgHandler : public Singleton<QtMsgHandler>
  {
    friend class Singleton<QtMsgHandler>;
  public:
      /**
        * Install a callback that should be called when a qFatal arrives.
        * Use to do interesting stuff before your app goes down.
        * @param a_pCallback the new Callback or 0 for none
        */
    void mp_InstallFatalCallback( streamapp::Callback* a_pCallback );

  protected:
      /**
        * The message handler function.
        * @param a_Type the type, eg QtDebugMsg
        * @param ac_psMsg the text
        */
    static void sf_MessageOutput( QtMsgType a_Type, const char* ac_psMsg );

      /**
        * The callback for qFatals.
        */
    static streamapp::Callback* sm_pFatalCb;

      /**
        * Private Constructor.
        * Sets the new messagehandler.
        */
    QtMsgHandler();

    /**
      * Private Destructor.
      * Restores all message handling.
      */
    ~QtMsgHandler();
  };

    /**
      * QtMsgCatcher
      *   catches al std::cout/std::cerr/qDebug/etc and
      *   emits ms_Message() for it.
      *   Uses both CallbackStream and QtMsgHandler.
      *   Usage: use once early in the program, eg in in main() :
      *
      *   @code
      *   QtMsgCatcher* pCatch = QtMsgCatcher::sf_pInstance();
      *     //question marks stand for a class that will handle the messages,
      *     //eg MessageWindow is very appropriate.
      *   connect( pCatch, SIGNAL( ms_Message( const QString& ) ), ???, ??? );
      *   @endcode
      *
      *   @see CallbackStream
      *   @see QtMsgHandler
      ************************************************** */
  class QtMsgCatcher : public QObject, public Singleton<QtMsgCatcher>
  {
      Q_OBJECT
    friend class Singleton<QtMsgCatcher>;
  public:
      /**
        * Set to emit ms_Message( const QString& ) or ms_Message( const QString&, const gt_eMsgType ).
        * @param ac_bSet false for plain messages
        */
    void mp_SetEmitType( const bool ac_bSet = true )
    { mv_bEmitType = ac_bSet; }

  signals:
      /**
        * Emitted when ms_DoMessage() is called and mp_SetEmitType() is false.
        */
    void ms_Message( const QString& );

      /**
        * Emitted when ms_DoMessage() is called and mp_SetEmitType() is true (default).
        * std::cout is treated as gc_eMessage, cerr as gc_eError.
        */
    void ms_Message( const QString&, const streamapp::gt_eMsgType& );

  public slots:
      /**
        * Emits ms_Message().
        * Normally called from static cout redirector function.
        */
    void ms_DoMessage( const QString&, const streamapp::gt_eMsgType& );

  protected:
      /**
        * The Qt message handler function.
        * @param a_Type the type, eg QtDebugMsg
        * @param ac_psMsg the text
        */
    static void sf_QtMsgHandler( QtMsgType a_Type, const char* ac_psMsg );

      /**
        * Private Constructor.
        * Redirects all message handling to this class' instance.
        */
    QtMsgCatcher();

    /**
      * Private Destructor.
      * Restores all message handling.
      */
    ~QtMsgCatcher();

    bool mv_bEmitType;
    utils::CallbackStream<> m_OutRedir;
    utils::CallbackStream<> m_ErrRedir;
  };

}

#endif //#ifndef __QT_MESSAGEREDIRECTOR_H__
