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
 
#include "eventsocketconn.h"
#include "utils/tracer.h"

using namespace network;

EventSocketConnection::EventSocketConnection( const unsigned ac_nMagicID, const bool ac_bNegotiate ) :
  ISocketConnection( ac_nMagicID, ac_bNegotiate )
{
}

EventSocketConnection::~EventSocketConnection()
{
}

void EventSocketConnection::mp_DispatchEvent( const gt_eSockConnEvent& ac_tEvent, gt_EventData a_pData )
{
  if( ac_tEvent == gc_eConnect )
  {
    mt_ConnectionInfo* p = (mt_ConnectionInfo*) a_pData;
    if( !ISocketConnection::mp_bConnectToSocket( p->m_nAddress, p->m_nPort, p->m_nTimeout ) )
      IEventCommunicator<gt_eSockConnEvent>::mp_DispatchEventToListener( gc_eError );
  }
  else if( ac_tEvent == gc_eTxMessage )
  {
    const RawMemoryAccess* p = (RawMemoryAccess*) a_pData;
    if( !ISocketConnection::mf_bSendMessage( *p ) )
      IEventCommunicator<gt_eSockConnEvent>::mp_DispatchEventToListener( gc_eError );
  }
  else if( ac_tEvent == gc_eDisConnect )
  {
    ISocketConnection::mp_DisConnect();
  }
  else if( ac_tEvent == gc_eQueryID )
  {
    unsigned* p = (unsigned*) a_pData;
    *p = mf_nGetID();
  }
}

void EventSocketConnection::mf_MessageReceived( const RawMemoryAccess& ac_Data )
{
  RawMemoryAccess* p = const_cast<RawMemoryAccess*>( &ac_Data );
  IEventCommunicator<gt_eSockConnEvent>::mp_DispatchEventToListener( gc_eRxMessage, p );
}

void EventSocketConnection::mf_Connected()
{
  IEventCommunicator<gt_eSockConnEvent>::mp_DispatchEventToListener( gc_eConnect );
}

void EventSocketConnection::mf_DisConnected()
{
  IEventCommunicator<gt_eSockConnEvent>::mp_DispatchEventToListener( gc_eDisConnect );
}

void EventSocketConnection::mf_Error( CommSocket::mt_eStatus a_eErrorCode )
{
  IEventCommunicator<gt_eSockConnEvent>::mp_DispatchEventToListener( gc_eCommError, &a_eErrorCode );
}
