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
 
#include "socketconn.h"
#include "socketfunc.h"
#include "appcore/deleter.h"
#include "appcore/threads/locks.h"
#include "containers/rawmemory.h"
#include "utils/tracer.h"
#include "tcpsocket.h"

using namespace network;

ISocketConnection::ISocketConnection( const unsigned ac_nID, const bool ac_bNegotiate ) :
    IThread( "ISC client" ),
  m_nMagicID( ac_nID ),
  mc_bNegotiate( ac_bNegotiate ),
  m_pSocket( 0 ),
  m_pCommSocket( 0 ),
  mc_Lock()
{
}

ISocketConnection::~ISocketConnection()
{
  if( m_pSocket )
  {
    m_pSocket->mf_bClose();
    delete m_pSocket;
  }
  delete m_pCommSocket;
}

bool ISocketConnection::mp_bConnectToSocket( const int ac_nAddress, const unsigned short ac_nPort, int a_nTimeOut )
{
  mp_DisConnect();

  const Lock L( mc_Lock );
  m_pSocket = new TcpSocket( ac_nAddress );
  if( m_pSocket->mf_bOpen( ac_nPort, a_nTimeOut ) )
  {
    if( !m_pCommSocket )
      m_pCommSocket = new CommSocket( m_pSocket, false );
    else
      m_pCommSocket->mp_SetSocket( m_pSocket );
    if( mc_bNegotiate )
    {
      DBG( "requesting id" );
      if( !mp_bNegotiation() )
        return false;
    }
    DBGPF( "my id is %0X", m_nMagicID );
    m_pCommSocket->mp_UseMagicNumber( m_nMagicID );
    mf_Connected();
    mp_Start();
    return true;
  }
  else
  {
    ZeroizingDeleter()( m_pSocket );
    mf_DisConnected();
    return false;
  }
}

bool ISocketConnection::mp_bNegotiation()
{
    //wait until the other side send us it's ID
  for( ;; )
  {
    int nState = m_pSocket->mf_nIsReady( 200 );
    if( nState < 0 )
    {
        //serious problem here
      mf_Error( CommSocket::mc_eSockNoC );
      mf_DisConnected();
      ZeroizingDeleter() ( m_pSocket );
      return false;
    }
    else if( nState > 0 )
    {
        //should be an id..
      m_pCommSocket->mp_NoMagicNumber();
      RawMemory mem;
      CommSocket::mt_eStatus status = m_pCommSocket->mf_eReadFromSocket( mem );
      if( status != CommSocket::mc_eOk )
      {
        mf_Error( status );
        mf_DisConnected();
        ZeroizingDeleter()( m_pSocket );
        return false;
      }
      s_assert( mem.mf_nGetSize() == sizeof( m_nMagicID ) );
      m_nMagicID = * ( mem.mf_pGetMemory<unsigned>() );
      return true;
    }
  }
}

bool ISocketConnection::mf_bIsConnected() const
{
  const Lock L( mc_Lock );
  return ( m_pSocket && m_pSocket->mf_bIsOpen() && IThread::mf_bIsRunning() );
}

void ISocketConnection::mp_DisConnect()
{
  if( m_pSocket )
  {
    m_pSocket->mf_bClose();
    mf_DisConnected();
  }

  IThread::mp_Stop( 4000 );

  const Lock L( mc_Lock );
  ZeroizingDeleter()( m_pSocket );
}

bool ISocketConnection::mf_bSendMessage( const RawMemoryAccess& ac_Data )
{
  return mf_bSendMessage( ac_Data, ac_Data.mf_nGetSize() );
}

bool ISocketConnection::mf_bSendMessage( const RawMemoryAccess& ac_Data, const unsigned ac_nBytesToSend )
{
  if( !m_pSocket || !m_pSocket->mf_bIsOpen() )
    return false;

  const Lock L( mc_Lock );
  CommSocket::mt_eStatus status = m_pCommSocket->mf_eWriteToSocket( ac_Data, ac_nBytesToSend );
  if( status != CommSocket::mc_eOk )
  {
    mf_Error( status );
    return false;
  }
  return true;
}

bool ISocketConnection::mf_bReadMessage()
{
  if( !m_pSocket || !m_pSocket->mf_bIsOpen() )
    return false;

  const Lock L( mc_Lock );
  RawMemory mem;
  CommSocket::mt_eStatus status = m_pCommSocket->mf_eReadFromSocket( mem );
  if( status != CommSocket::mc_eOk )
  {
    mf_Error( status );
    mf_DisConnected();
    ZeroizingDeleter()( m_pSocket );
    return false;
  }
  mf_MessageReceived( mem );
  return true;
}

void ISocketConnection::mp_Run()
{
#ifdef S_C6X
    //all threads @C6X using networking need this,
    //else not much of a connection can be made
  if( !fdOpenSession( TaskSelf() ) )
    s_assert( 0 );
#endif
  while( !IThread::mf_bThreadShouldStop() )
  {
    if( m_pSocket )
    {
      const int nReadyState = m_pSocket->mf_nIsReady();

      if( nReadyState < 0 )
      {
        mf_Error( CommSocket::mc_eSockNoC );
        const Lock L( mc_Lock );
        ZeroizingDeleter()( m_pSocket );
        mf_DisConnected();
        break;
      }
      else if( nReadyState > 0 )
      {
        if( !mf_bReadMessage() )
          break;
      }
      else
      {
        IThread::sf_Sleep( 5 );
      }
    }
    else
    {
      break;
    }
  }
#ifdef S_C6X
  fdCloseSession( TaskSelf() );
#endif
}

void ISocketConnection::mp_InitWithSocket( ISocket* const ac_pSocket )
{
  s_assert( ac_pSocket );
  m_pSocket = ac_pSocket;
  if( !m_pCommSocket )
    m_pCommSocket = new CommSocket( m_pSocket, false );
  else
    m_pCommSocket->mp_SetSocket( m_pSocket );
  if( !mc_bNegotiate )
  {
    m_pCommSocket->mp_UseMagicNumber( m_nMagicID );
  }
  else
  {
    DBG( "sending id" );
      //send our ID
    RawMemory mem;
    mem.mp_Append( &m_nMagicID, sizeof( m_nMagicID ) ); //temp disable it, else we cannot communicate
    m_pCommSocket->mp_NoMagicNumber();
    if( !mf_bSendMessage( mem ) )
    {
      mp_DisConnect();  //this should never be reached, if it is, there's something wrong with the server!
      mf_DisConnected();
    }
    m_pCommSocket->mp_UseMagicNumber( m_nMagicID );     //enable again
  }
  DBGPF( "my id is %0X", m_nMagicID );
  mf_Connected();
  mp_Start();
}
