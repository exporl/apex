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
 
#ifndef __EVENTSOCKETCONNMGR_H__
#define __EVENTSOCKETCONNMGR_H__

#include "eventsocketconn.h"
#include "socketconnfactoryimp.h"
#include "appcore/events/events.h"

using namespace appcore;

namespace network
{

    /**
      * ISocketConnManager
      *   SocketConnectionFactory implementation for EventSocketConn.
      *************************************************************** */
  class EventSocketConnManager : public SocketConnectionFactory
  {
  public:
      /**
        * Constructor.
        */
    EventSocketConnManager();

      /**
        * Destructor.
        */
    ~EventSocketConnManager();

      /**
        * Implementation of the SocketConnectionFactory method.
        */
    ISocketConnection* mf_pCreateConnectionImp( const unsigned ac_nMagicID, const bool ac_bNegotiate );

      /**
        * Implementation of the SocketConnectionFactory method.
        */
    void mf_AboutToRemove( ISocketConnection* a_pConnection );

      /**
        * Install the router to register new connections with.
        * Connections are registered to dispatch everything
        * to the router, and the connections are registered
        * with the router to receive gc_eTxMessages.
        * The last one means that *every* connection will receive
        * any gc_eTxMessage, broadcast-style, so
        * Abandoned connections are unregistered again.
        * @param ac_pRouter the router
        */
    void mp_InstallEventDispatcher( IEventRouter<gt_eSockConnEvent>* const ac_pRouter );

  private:
    typedef OwnedArray<EventSocketConnection> mt_ConnList;

    IEventRouter<gt_eSockConnEvent>* m_pRouter;
  };

}

#endif //#ifndef __EVENTSOCKETCONNMGR_H__
