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
 
#ifndef __SOCKETCONNEVENT_H__
#define __SOCKETCONNEVENT_H__

#include "socketconn.h"
#include "socketconnserver.h"
#include "appcore/singleton.h"
#include "appcore/events/events.h"
using namespace appcore;

namespace network
{

    /**
      * Event types used by EventSocketConnection.
      */
  enum gt_eSockConnEvent
  {
    gc_eError,
    gc_eCommError,
    gc_eConnect,
    gc_eDisConnect,
    gc_eRxMessage,
    gc_eTxMessage,
    gc_eQueryID
  };

    /**
      * EventSocketConnection
      *   an ISocketConnection implementation for events.
      *   Use this if you use events: dispatching a gc_eTxMessage equals
      *   calling mf_bSendMessage. The EventSocketConnection callbacks
      *   call PostEvent with the corresponding gt_eSockConnEvent and
      *   eventually the data.
      *   If there is no listener installed, nothing is posted.
      ******************************************************************* */
  class EventSocketConnection : public ISocketConnection, public IEventCommunicator<gt_eSockConnEvent>
  {
  public:
      /**
        * Constructor.
        * Doesn't connect, just sets up.
        * @param ac_nMagicID message ID used in header to check validity.
        * The other end should use the same number in order to be recognized.
        */
    EventSocketConnection( const unsigned ac_nMagicID = 0, const bool ac_bNegotiate = true );

      /**
        * Destructor.
        */
    ~EventSocketConnection();

      /**
        * mt_ConnectionInfo
        *   container for mp_bConnectToSocket() arguments.
        ************************************************** */
    struct mt_ConnectionInfo
    {
      int m_nAddress;
      unsigned short m_nPort;
      int m_nTimeout;
    };

      /**
        * Dispatch an event.
        * gc_eConnect connects, use a mt_ConnectionInfo*
        * gc_eDisconnect disconnects, no data required
        * gc_eTxMessage sends a message, use a const RawMemory*
        */
    void mp_DispatchEvent( const gt_eSockConnEvent& ac_tEvent, gt_EventData a_pData = 0 );

  protected:
      /**
        * Implementation of the ISocketConnection method.
        */
    void mf_MessageReceived( const RawMemoryAccess& ac_Data );

      /**
        * Implementation of the ISocketConnection method.
        */
    void mf_Connected();

      /**
        * Implementation of the ISocketConnection method.
        */
    void mf_DisConnected();

      /**
        * Implementation of the ISocketConnection method.
        */
    void mf_Error( CommSocket::mt_eStatus a_eErrorCode );
  };

}

#endif //#ifndef __SOCKETCONNEVENT_H__
