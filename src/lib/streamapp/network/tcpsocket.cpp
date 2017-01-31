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
 
#include "tcpsocket.h"
#include "appcore/threads/thread.h"

#ifdef S_POSIX

  #include <string.h>
  #include <sys/socket.h>
  #include <sys/types.h>
  #include <arpa/inet.h>
  #include <sys/errno.h>
  #include <netinet/tcp.h>
  #include <netinet/in.h>
  #include <netdb.h>
  #include <fcntl.h>

  namespace
  {
    const int INVALID_SOCKET = -1;
  }

#elif defined S_PSP

  #include <sys/socket.h>
  #include <sys/select.h>
  #include <sys/fd_set.h>
  #include <sys/types.h>
  #include <netinet/in.h>
  #include <sys/errno.h>

  namespace
  {
    const int INVALID_SOCKET = -1;
  }

  #define socket      sceNetInetSocket
  #define connect     sceNetInetConnect
  #define bind        sceNetInetBind
  #define listen      sceNetInetListen
  #define accept      sceNetInetAccept
  #define recv        sceNetInetRecv
  #define send        sceNetInetSend
  #define setsockopt  sceNetInetSetsockopt
  #define select      sceNetInetSelect

#elif defined S_WIN32

  #ifdef _MSC_VER
    #pragma warning(disable:4127)
  #endif

#endif

#ifndef IPPROTO_TCP
  #define IPPROTO_TCP 6
#endif

using namespace network;


/******************************************************************************/
          //Start WSA on windows

#ifdef S_WIN32

#include <appcore/singleton.h>

namespace
{
  class WsaStarter : public appcore::Singleton<WsaStarter>
  {
    friend class appcore::Singleton<WsaStarter>;
  protected:
    WsaStarter()
    {
      WSAData wsaData;
      if( WSAStartup( MAKEWORD( 1, 1 ), &wsaData ) !=  0 )
        throw( 0 ); //we cannot have this
    }

  public:
    ~WsaStarter()
    {
      WSACleanup();
    }
  };
}

#endif

/******************************************************************************/

TcpSocket::TcpSocket( const int ac_nAddress ) :
  mc_nAddress( ac_nAddress ),
  mv_bIsListener( false ),
  m_hSocket( INVALID_SOCKET )
{
#ifdef S_WIN32
  WsaStarter::sf_pInstance();
#endif
}

TcpSocket::TcpSocket( const int ac_nAddress, SOCKET a_Sock ) :
  mc_nAddress( ac_nAddress ),
  mv_bIsListener( false ),
  m_hSocket( a_Sock )
{
#ifdef S_WIN32
  WsaStarter::sf_pInstance();
#endif
}

TcpSocket::~TcpSocket()
{
  mf_bClose();
}

bool TcpSocket::mf_bOpen( const unsigned short ac_nPort, int a_nTimeOut )
{
  if( !mf_bIsOpen() && !mv_bIsListener )
  {
      //create the socket
    m_hSocket  = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if( m_hSocket == INVALID_SOCKET )
      return false;

      //set port and address
    struct sockaddr_in addrListenIn;
    memset( &addrListenIn, 0, sizeof( addrListenIn ));
    addrListenIn.sin_family       = AF_INET;
    addrListenIn.sin_port         = htons( ac_nPort );
    addrListenIn.sin_addr.s_addr  = (unsigned long) mf_nGetAddress();

      //allow reuse
    int one = 1;

#ifdef S_PSP
    const char* pOne = (const char*) &one;
#else
    char* pOne = (char*) &one;
#endif
    setsockopt( m_hSocket, SOL_SOCKET, SO_REUSEADDR, pOne, sizeof( int ) );

    int err = -1;

      //try connect
    if( a_nTimeOut == 0 )
    {
      err = connect( m_hSocket, (sockaddr*) &addrListenIn, sizeof( addrListenIn ) );
    }
    else
    {
      while( a_nTimeOut != 0 )
      {
        err = connect( m_hSocket, (sockaddr*) &addrListenIn, sizeof( addrListenIn ) );

        if( err >= 0 )
          break;

        if( a_nTimeOut > 0 )
        {
          const int nSleepy = s_min( a_nTimeOut, 1000 );
          a_nTimeOut -= nSleepy;
          appcore::IThread::sf_Sleep( nSleepy );
        }
      }
    }

    if( err )
    {
      mf_bClose();
      return false;
    }
    else
    {
      mv_nPort = ac_nPort;
      return true;
    }
  }
  return false;
}

bool TcpSocket::mf_bCreateListener( const unsigned short ac_nPort )
{
  if( mf_bIsOpen() )
    mf_bClose();

  if( !mf_bIsOpen() )
  {
      //Create TCP socket for listening
    m_hSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if( m_hSocket == INVALID_SOCKET )
      return false;

      //Enable address + port reuse
      //if we don't do this, we have to wait 5 minutes or so before the socket's state is free!!
    int one = 1;
#ifdef S_PSP
    const char* pOne = (const char*) &one;
#else
    char* pOne = (char*) &one;
#endif
    setsockopt( m_hSocket, SOL_SOCKET, SO_REUSEADDR, pOne, sizeof( int ) );

      //Prepare address for connect
    struct sockaddr_in sin2;
    memset( &sin2, 0, sizeof( struct sockaddr_in ) );
    sin2.sin_family       = AF_INET;
    sin2.sin_port         = htons( ac_nPort );
    sin2.sin_addr.s_addr  = (unsigned long) mf_nGetAddress();

      //Bind socket to address
    if( bind( m_hSocket, (sockaddr*) &sin2, sizeof( sin2 ) ) < 0 )
      return false;

      //Setup as listening
    if( listen( m_hSocket, sc_nMaxNumListened ) < 0 )
      return false;

    mv_bIsListener = true;

    mv_nPort = ac_nPort;

      //ready
    return mf_bIsOpen();
  }
  return false;
}

ISocket* TcpSocket::mf_pOpenFromListener()
{
  if( !mf_bIsOpen() || !mv_bIsListener )
    return 0;

  struct sockaddr_in sin2;
#if defined (S_POSIX) || defined (S_PSP)
  socklen_t size = sizeof( sin2 );
#else
  int size = sizeof( sin2 );
#endif

    //Wait for connection (endless)
  SOCKET newSock = accept( m_hSocket, (sockaddr*) &sin2, &size );
  
  if( newSock != INVALID_SOCKET )
    return new TcpSocket( mf_nGetAddress(), newSock );
  return 0;
}

bool TcpSocket::mf_bClose()
{
  if( m_hSocket == INVALID_SOCKET )
    return false;
    
  if( !mv_bIsListener )
  {
#ifndef S_PSP
    shutdown( m_hSocket, 2 );
#endif
#if defined (S_WIN32)
    closesocket( m_hSocket );
#elif defined (S_POSIX)
    close( m_hSocket );
#elif defined (S_C6X)
    fdClose( m_hSocket );
#elif defined (S_PSP)
    sceNetInetShutdown( m_hSocket, 2 );
    sceNetInetClose( m_hSocket );
#endif
  }
  else
  {
#ifndef S_PSP
    shutdown( m_hSocket, 0 );
#endif
#if defined (S_WIN32)
    closesocket( m_hSocket );
#elif defined (S_POSIX)
    close( m_hSocket );
#elif defined (S_C6X)
    fdClose( m_hSocket );
#elif defined (S_PSP)
    sceNetInetShutdown( m_hSocket, 0 );
    sceNetInetClose( m_hSocket );
#endif
  }
  m_hSocket = INVALID_SOCKET;
  mv_nPort = 0;
  return true;
}

bool TcpSocket::mf_bIsOpen() const
{
  return m_hSocket != INVALID_SOCKET;
}

int TcpSocket::mf_nIsReady( const int ac_nTimeOut )
{
  if( !mf_bIsOpen() )
    return -1;

#ifndef S_C6X

    struct timeval ttimeout;
    struct timeval* ptimeout;
    fd_set readbits;

    if( ac_nTimeOut >= 0 )
    {
      ttimeout.tv_sec = ac_nTimeOut / 1000;
      ttimeout.tv_usec = ( ac_nTimeOut % 1000 ) * 1000;
      ptimeout = &ttimeout;
    }
    else
    {
      ptimeout = 0;
    }

    FD_ZERO( &readbits );
    FD_SET( m_hSocket, &readbits );

  #ifdef S_WIN32
    if( select ( 1, &readbits, 0, 0, ptimeout ) < 0 )
      return -1;
  #else
    int result;
    while( ( result = select( m_hSocket + 1, &readbits, 0, 0, ptimeout ) ) < 0 && errno == EINTR )
    {
    }
    if( result < 0 )
      return -1;
  #endif

    if( FD_ISSET( m_hSocket, &readbits ) )
      return 1;

#else

      //fdPoll() is lower level but faster then fdSelect()
    FDPOLLITEM item;
    item.fd = m_hSocket;
    item.eventsRequested = POLLIN;
    if( fdPoll( &item, 1, ac_nTimeOut ) < 0 )
      return -1;
    else if( item.eventsDetected == 1 )
      return 1;

#endif

  return 0;
}

int TcpSocket::mf_nRead( void* a_pBuf, const int ac_nBytes ) const
{
  if( mf_bIsOpen() )
  {
#ifndef S_PSP
    int nRx = recv( m_hSocket, (char*) a_pBuf, ac_nBytes, 0 );
#else
    int nRx = recv( m_hSocket, (u8*) a_pBuf, ac_nBytes, 0 );
#endif
    return nRx;
  }
  return 0;
}

int TcpSocket::mf_nWrite( void* a_pBuf, const int ac_nBytes ) const
{
  if( mf_bIsOpen() )
  {
#ifndef S_PSP
    int nTx = send( m_hSocket, (char*) a_pBuf, ac_nBytes, 0 );
#else
    int nTx = send( m_hSocket, a_pBuf, ac_nBytes, 0 );
#endif
    return nTx;
  }
  return 0;
}

int TcpSocket::mf_nGetLastError() const
{
#if defined (S_WIN32)
  return WSAGetLastError();
#elif defined (S_POSIX)
  return 0; //where is this call??
#elif defined (S_C6X)
  return fdError();
#elif defined (S_PSP)
  return sceNetInetGetErrno();
#endif
}

