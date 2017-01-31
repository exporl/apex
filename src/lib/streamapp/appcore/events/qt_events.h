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

#ifndef __QTEVENTS_H_
#define __QTEVENTS_H_

#include "events.h"

#include <QApplication>

namespace appcore
{

    /**
      * QDispatchedEvent
      *   Qt wrapper for appcore's events.
      *   The sender can set the delete flag
      *   to inform the receiver it must delete the data
      *   However, the event itself must not be deleted by the receiver
      *   when using postEvent (this is done by Qt's event queue ),
      *   but it must be deleted when using sendEvent!
      *   @note since appcore's events are asynchronous, a_pData must
      *   remain valid until the event is received!!
      *   @see QtEventListener
      ***************************************************************** */
  template<class tType = int>
  class QDispatchedEvent :
#if QT_VERSION < 0x040000
    public QCustomEvent
#else
    public QEvent
#endif
  {
  public:
      /**
        * Constructor.
        * @param ac_eEventType the event type
        * @param a_pData the event data
        * @param ac_bReceiverMustDelete if true, the receiver should delete the m_pData mamber (== a_pData)
        */
    QDispatchedEvent( const tType& ac_eEventType, gt_EventData a_pData = 0, const bool ac_bReceiverMustDelete = false ) :
#if QT_VERSION < 0x040000
        QCustomEvent( sc_QDispatchedEventType ),
#else
        QEvent( sc_QDispatchedEventType ),
#endif
      mc_bReceiverMustDelete( ac_bReceiverMustDelete ),
      mc_nEventType( ac_eEventType ),
      m_pData( a_pData )
    {
    }

      /**
        * Set the type for QEvent.
        * If QEvent::type() == sc_QDispatchedEventType,
        * one can safely cast it to a QDispatchedEvent.
        */
    static const QEvent::Type sc_QDispatchedEventType = (QEvent::Type) 65500;

    const bool          mc_bReceiverMustDelete;

    const tType         mc_nEventType;    //!< appcore's event code
    const gt_EventData  m_pData;          //!< appcore's event data

  private:
    QDispatchedEvent( const QDispatchedEvent& );
    QDispatchedEvent& operator = ( const QDispatchedEvent& );
  };

    /**
      * QtEventListener
      *   translate appcore's event dispatching to Qt's event mechanism.
      *   Every mp_DispatchEvent() call simply wraps the code and data into
      *   a QDispatchedEvent, and calls QApplication::postEvent on the target.
      *   @see QDispatchedEvent
      ************************************************************************ */
  template<class tType = int>
  class QtEventListener : public IEventDispatcher<tType>
  {
  public:
      /**
        * Constructor.
        * @param a_pQEventTarget the QObject to post events to
        */
    QtEventListener( QObject* a_pQEventTarget ) :
      m_pTarget( a_pQEventTarget )
    {}

      /**
        * Destructor.
        */
    ~QtEventListener()
    {}

      /**
        * Set a(nother) target.
        * @param a_pQEventTarget the QObject to post events to
        */
    void mp_SetTarget( QObject* a_pQEventTarget )
    { m_pTarget = a_pQEventTarget; }

      /**
        * Dispatch an event and forward it to the target.
        * Doesn't do anything if target is 0.
        * @param ac_eEventType the event code
        * @param a_pData the event data
        */
    void mp_DispatchEvent( const tType& ac_eEventType, gt_EventData a_pData = 0 )
    {
      if( m_pTarget )
        QApplication::postEvent( m_pTarget, new QDispatchedEvent<tType>( ac_eEventType, a_pData ) );
    }

  private:
    QObject* m_pTarget;

    QtEventListener( const QtEventListener& );
    QtEventListener& operator = ( const QtEventListener& );
  };

}

#endif //__QTEVENTS_H_
