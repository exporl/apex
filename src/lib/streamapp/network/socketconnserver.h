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
 
#ifndef __SOCKETCONNSERVER_H__
#define __SOCKETCONNSERVER_H__

#include "appcore/threads/thread.h"
using namespace appcore;

namespace network
{

  class ISocket;
  class ISocketConnection;
  class ISocketConnectionFactory;

    /**
      * ISocketConnectionServer
      *   server side for ISocketConnection.
      *   After listening started, spawns new ISocketConnection objects
      *   to serve the connection with the remote side.
      *   The connections are acquired through the given factory only.
      *   The factory is responsible for deleting the connections that
      *   are not used (aka disconnected) anymore.
      ********************************************************************* */
  class ISocketConnectionServer : private IThread
  {
  public:
      /**
        * Constructor.
        * @param ac_pConnFactory the factory for connections to use
        */
    ISocketConnectionServer( ISocketConnectionFactory* const ac_pConnFactory );

      /**
        * Destructor.
        */
    ~ISocketConnectionServer();

      /**
        * Start listening for connections on the given port.
        * This will start the server thread, which will spawn
        * new connections everytime a remote socket connects to the port.
        * mf_pCreateConnection() is called to create a connection with the client.
        * Returns immedeatley if there was an error,
        * else use mp_Stop() to stop waiting.
        * @param ac_nPortNumber the port number to use
        * @return false if there was an error
        */
    bool mp_bStartListening( const short ac_nPortNumber );

      /**
        * Stop the server thread.
        * Doesn't do much if not started in the first place.
        */
    void mp_Stop();

  private:
    ISocket* m_pSocket;
    ISocketConnectionFactory* const mc_pFactory;

      /**
        * Implementation of the IThread method.
        */
    void mp_Run();
  };

}

#endif //#ifndef __SOCKETCONNSERVER_H__
