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
 
#include "socketconnserver.h"
#include "socketconn.h"
#include "socketconnfactory.h"
#include "tcpsocket.h"
#include "appcore/deleter.h"

using namespace network;

ISocketConnectionServer::ISocketConnectionServer( ISocketConnectionFactory* const ac_pConnFactory ) :
    IThread( "ISC server" ),
  m_pSocket( 0 ),
  mc_pFactory( ac_pConnFactory )
{
}

ISocketConnectionServer::~ISocketConnectionServer()
{
  mp_Stop();
}

bool ISocketConnectionServer::mp_bStartListening( const short ac_nPort )
{
  m_pSocket = new TcpSocket( 0 );
  bool bRet = m_pSocket->mf_bCreateListener( ac_nPort );
  if( bRet )
    mp_Start();
  else
    ZeroizingDeleter() ( m_pSocket );
  return bRet;
}

void ISocketConnectionServer::mp_Run()
{
#ifdef S_C6X
    //all threads @C6X using networking need this,
    //else not much of a connection can be made
  if( !fdOpenSession( TaskSelf() ) )
    s_assert( 0 );
#endif
  while( !IThread::mf_bThreadShouldStop() && m_pSocket )
  {
    ISocket* const sockClient = m_pSocket->mf_pOpenFromListener();
    if( sockClient )
    {
      ISocketConnection* conn = mc_pFactory->mf_pCreateConnection( sockClient );
      if( !conn )
        delete sockClient;
    }
  }
#ifdef S_C6X
  fdCloseSession( TaskSelf() );
#endif
}

void ISocketConnectionServer::mp_Stop()
{
  IThread::mp_SetThreadShouldStop();

  if( m_pSocket )
    m_pSocket->mf_bClose();

  IThread::mp_Stop( 4000 );

  ZeroizingDeleter()( m_pSocket );
}
