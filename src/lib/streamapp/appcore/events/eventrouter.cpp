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
 
#include "eventrouter.h"
#include "appcore/threads/locks.h"
#include "appcore/threads/criticalsection.h"
#include <assert.h>

using namespace appcore;

EventRouter::EventRouter() :
  m_Dispatchers( 5 )
{
}

EventRouter::~EventRouter()
{
  m_Dispatchers.mp_RemoveAllItems();
}

void EventRouter::mp_DispatchEvent( const int& ac_eEventType, gt_EventData a_pData )
{
  if( mf_bPreDispatchEvent( ac_eEventType, a_pData ) )
    return;

  const unsigned nItems = mf_nGetNumDispatchers();
  for( unsigned i = 0 ; i < nItems ; ++i )
  {
    mt_DispatcherAndType* pCur = m_Dispatchers.mf_GetItem( i );
    if( pCur->mc_eType == ac_eEventType )
      pCur->mc_pDispatcher->mp_DispatchEvent( ac_eEventType, a_pData );
  }
}

void EventRouter::mp_RegisterEventDispatcher( DefEventDispatcher* a_pListener, const int& ac_eEventType )
{
  assert( a_pListener != this );
  m_Dispatchers.mp_AddItem( new mt_DispatcherAndType( ac_eEventType, a_pListener ) );
}

void EventRouter::mp_UnRegisterEventDispatcher( DefEventDispatcher* a_pListener, const int& ac_nEventType )
{
  assert( a_pListener != this );
  const unsigned nItems = mf_nGetNumDispatchers();
  for( unsigned i = 0 ; i < nItems ; ++i )
  {
    mt_DispatcherAndType* pCur = m_Dispatchers.mf_GetItem( i );
    if( pCur->mc_eType == ac_nEventType && pCur->mc_pDispatcher == a_pListener )
    {
      m_Dispatchers.mp_RemoveItemAt( i );
      break;
    }
  }
}

void EventRouter::mp_UnRegisterEventDispatcher( DefEventDispatcher* a_pListener )
{
  assert( a_pListener != this );
  for( unsigned i = 0 ; i < mf_nGetNumDispatchers() ; ++i )
  {
    mt_DispatcherAndType* pCur = m_Dispatchers.mf_GetItem( i );
    if( pCur->mc_pDispatcher == a_pListener )
    {
      m_Dispatchers.mp_RemoveItemAt( i );
      break;
    }
  }
}

unsigned EventRouter::mf_nGetNumDispatchers() const
{
  return m_Dispatchers.mf_nGetNumItems();
}
