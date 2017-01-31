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
 
#include "eventsocketconnlogger.h"
#include "utils/tracer.h"

using namespace network;

EventSocketConnLogger::EventSocketConnLogger()
{
}

EventSocketConnLogger::~EventSocketConnLogger()
{
}

void EventSocketConnLogger::mp_DispatchEvent( const gt_eSockConnEvent& ac_tType, gt_EventData a_pData )
{
  if( ac_tType == gc_eConnect )
    TRACE( "Connected" )
  else if( ac_tType == gc_eDisConnect )
    TRACE( "Disconnected" )
  else if( ac_tType == network::gc_eCommError )
  {
    network::CommSocket::mt_eStatus* p = (network::CommSocket::mt_eStatus*) a_pData;
    if( *p == network::CommSocket::mc_eMagicErr )
      TRACE( "Magic Error" )
    else if( *p == network::CommSocket::mc_eSockErrR )
      TRACE( "Read Error" )
    else if( *p == network::CommSocket::mc_eSockErrW )
      TRACE( "Write Error" )
    else
      TRACE( "Unk Error" )
  }
  else if( ac_tType == network::gc_eError )
  {
    TRACE( "Socket Error" )
  }
  else if( ac_tType == gc_eRxMessage )
  {
    TRACE( "RxMessage" )
  }
  else if( ac_tType == gc_eTxMessage )
  {
    TRACE( "TxMessage" )
  }
}
