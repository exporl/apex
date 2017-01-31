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
 
#ifndef __STR_SOCKET_H__
#define __STR_SOCKET_H__

#include "defines.h"

namespace network
{

    /**
      * ISocket
      *   interface for connection endpoints in a network.
      *   A connections endpoint is used to read and write data
      *   from and to a remote site, at which a same kind of endpoint
      *   does all the reverse actions. For example, if ISocket A wants
      *   to read, ISocket B has two write. Before this can happen, A has
      *   to be connected to B however. This also works using two counterparts,
      *   ones side has to listen for a connection, while the other side has
      *   to connect to the first one.
      *   Needless to say that both sites have to know and use the same
      *   protocol for communicating.
      *   Typical implementations include TCP/IP and UDP connections, but can
      *   as well be USB or so.
      ************************************************************************* */
  class ISocket
  {
  protected:
      /**
        * Protected Constructor.
        */
    ISocket()
    {
    }

  public:
      /**
        * Destructor.
        */
    virtual ~ISocket()
    {
    }

      /**
        * Open connection to remote site.
        * @param ac_nPort the local port
        * @param a_nTimeOut timeout interval in milliseconds (will be overwritten)
        * @return true on success
        */
    virtual bool mf_bOpen( const unsigned short ac_nPort, int a_nTimeOut = 0 ) = 0;

      /**
        * Get the port that was used for connecting.
        * Returns 0 if not connected yet.
        * @return the port number from the last connection made.
        */
    virtual unsigned short mf_nGetPortNumber() const = 0;

      /**
        * Use this connaction for listening.
        * After this call, the mf_pOpenFromListener() method
        * can be called repeatedly to get new connections.
        * A socket that is in listener mode cannot be used to
        * connect to another socket.
        * @param ac_nPort the local port to listen on
        * @return true on success
        */
    virtual bool mf_bCreateListener( const unsigned short ac_nPort ) = 0;

      /**
        * Wait for connections to this socket.
        * Only works after mf_bCreateListener().
        * The method waits forever for incoming connections.
        * After a connection was made, a connected ISocket is returned.
        * @return new ISocket which is ready for use.
        */
    virtual ISocket* mf_pOpenFromListener() = 0;

      /**
        * Check if socket is in listenere modus.
        * @return true if mf_bCreateListener() was called.
        */
    virtual bool mf_bIsListener() const  = 0;

      /**
        * Close the connection.
        * @return true if it succeeds
        */
    virtual bool mf_bClose() = 0;

      /**
        * Query connection.
        * @return true if the connection is open
        */
    virtual bool mf_bIsOpen() const = 0;

      /**
        * Query state.
        * For Tcp this will be implemented using select();
        * State can then be polled in a loop to see when interesting
        * stuff happens. To avoid polling, win32 has methods that detect
        * these events at kernel level and allow you to call back. Very nice,
        * but we have to keep it cross-platfrom so we won't use these.
        * @param ac_nTimeOutInMilliSeconds the timeout for the method
        * @return -1 = not connected, 0 = not ready, 1 = ready; 1 means you can get data from the socket.
        */
    virtual int mf_nIsReady( const int ac_nTimeOutInMilliSeconds = 0 ) = 0;

      /**
        * Read data.
        * @param a_pBuf the receive buffer
        * @param ac_nBytes the number of bytes to read
        * @return the number of bytes received
        */
    virtual int mf_nRead( void* a_pBuf, const int ac_nBytes ) const = 0;

      /**
        * Write data.
        * @param a_pBuf the data to send
        * @param ac_nBytes the number of bytes in a_pBuf
        * @return the number of bytes sent
        */
    virtual int mf_nWrite( void* a_pBuf, const int ac_nBytes ) const = 0;

      /**
        * Get the last error code.
        * @return the code
        */
    virtual int mf_nGetLastError() const = 0;
  };

}

#endif //#ifndef __STR_SOCKET_H__
