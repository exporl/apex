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
 
#include "eventsocketconnmgr.h"
#include "utils/tracer.h"
#include "utils/stringutils.h"

using namespace network;

EventSocketConnManager::EventSocketConnManager() :
  m_pRouter( 0 )
{
}

EventSocketConnManager::~EventSocketConnManager()
{
}

void EventSocketConnManager::mp_InstallEventDispatcher( IEventRouter<gt_eSockConnEvent>* const ac_pRouter )
{
  mf_ObtainLock();
  m_pRouter = ac_pRouter;
  mf_ReleaseLock();
}

ISocketConnection* EventSocketConnManager::mf_pCreateConnectionImp( const unsigned ac_nMagicID, const bool ac_bNegotiate )
{
  EventSocketConnection* pNew = new EventSocketConnection( ac_nMagicID, ac_bNegotiate );
  if( m_pRouter )
  {
    pNew->mp_InstallEventDispatcher( m_pRouter );
    m_pRouter->mp_RegisterEventDispatcher( pNew, gc_eTxMessage );
  }
  return pNew;
}

void EventSocketConnManager::mf_AboutToRemove( ISocketConnection* a_pConnection )
{
  if( m_pRouter )
    m_pRouter->mp_UnRegisterEventDispatcher( (EventSocketConnection*) a_pConnection );
}
