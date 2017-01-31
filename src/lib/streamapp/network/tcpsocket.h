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
 
#ifndef __TCPSOCKET_H__
#define __TCPSOCKET_H__

#include "socket.h"
#include "defines.h"

#if defined S_WIN32
#include <WinSock.h>
#elif defined S_C6X
#include <netmain.h>
#elif defined S_POSIX
typedef int SOCKET;
#elif defined S_PSP
#define SOCKET int
#else
#ERROR !no platform defined!
#endif

  /**
    * namespace for network-related classes
    *************************************** */
namespace network
{

    /**
      * TcpSocket
      *   class representing a tcp socket.
      *   Just implements all ISocket functions
      *   using Berkely Tcp sockets.
      *   @see ISocket
      ***************************************** */
  class TcpSocket : public ISocket
  {
  public:
      /**
        * Constructor.
        * @param ac_nAddress the adress of the adapter to connect on
        */
    TcpSocket( const int ac_nAddress );

      /**
        * Destructor.
        */
    virtual ~TcpSocket();

      /**
        * Get the connect address.
        * @return the adress
        */
    INLINE const int& mf_nGetAddress() const
    {
      return mc_nAddress;
    }

      /**
        * Implementation of the ISocket method.
        */
    INLINE unsigned short mf_nGetPortNumber() const
    {
      return mv_nPort;
    }

      /**
        * Implementation of the ISocket method.
        */
    bool mf_bOpen( const unsigned short ac_nPort, int a_nTimeOut = 0 );

      /**
        * Implementation of the ISocket method.
        */
    bool mf_bCreateListener( const unsigned short ac_nPort );

      /**
        * Implementation of the ISocket method.
        */
    ISocket* mf_pOpenFromListener();

      /**
        * Implementation of the ISocket method.
        */
    INLINE bool mf_bIsListener() const
    {
      return mv_bIsListener;
    }

      /**
        * Implementation of the ISocket method.
        */
    bool mf_bClose();

      /**
        * Implementation of the ISocket method.
        */
    bool mf_bIsOpen() const;

      /**
        * Implementation of the ISocket method.
        */
    int mf_nIsReady( const int ac_nTimeOutInMilliSeconds = 0 );

      /**
        * Implementation of the ISocket method.
        */
    int mf_nRead( void* a_pBuf, const int ac_nBytes ) const;

      /**
        * Implementation of the ISocket method.
        */
    int mf_nWrite( void* a_pBuf, const int ac_nBytes ) const;

      /**
        * Implementation of the ISocket method.
        */
    int mf_nGetLastError() const;

    static const int sc_nMaxNumListened = 1;

  private:
    const int       mc_nAddress;
    unsigned short  mv_nPort;
    bool            mv_bIsListener;
    SOCKET          m_hSocket;

    TcpSocket( const int ac_nAddress, SOCKET a_Socket );
    TcpSocket( const TcpSocket& );
    TcpSocket& operator = ( const TcpSocket& );
  };

}

#endif //#ifndef __TCPSOCKET_H__
