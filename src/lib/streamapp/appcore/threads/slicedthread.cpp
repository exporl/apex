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
 
#include "slicedthread.h"
#include "locks.h"

using namespace appcore;

SlicedThread::SlicedThread( const String& ac_sName ) :
    IThread( ac_sName ),
  mv_bNotify( false ),
  mv_nCurIndex( 0 ),
  mv_nIdleTime( 500 ),
  mv_pCurTask( 0 )
{
}

SlicedThread::~SlicedThread()
{
  IThread::mp_Stop( 10000 );
}

void SlicedThread::mp_RegisterClientTask( SlicedThreadTask* const ac_pTask )
{
  const Lock L( mc_ListLock );
  m_TaskList.mp_AddItemIfNotThere( ac_pTask );
  mv_bNotify = true;
  IThread::mf_NotifyWait();
}

void SlicedThread::mp_UnRegisterClientTask( SlicedThreadTask* const ac_pTask )
{
  mc_ListLock.mf_Enter();
  if( mv_pCurTask == ac_pTask )
  {
    mc_ListLock.mf_Leave();
    const Lock L1( mc_TaskLock );
    const Lock L2( mc_ListLock );
    m_TaskList.mp_RemoveItem( ac_pTask );
  }
  else
  {
    m_TaskList.mp_RemoveItem( ac_pTask );
    mc_ListLock.mf_Leave();
  }
  mv_bNotify = true;
}

unsigned SlicedThread::mf_nGetNumTasks() const
{
  return m_TaskList.mf_nGetNumItems();
}

SlicedThreadTask* SlicedThread::mf_pGetTask(const unsigned ac_nIndex )
{
  const Lock L( mc_ListLock );
  return m_TaskList.mf_GetItem( ac_nIndex );
}

void SlicedThread::mp_SetMaxIdleTime(const unsigned ac_nTimeInMilliSeconds )
{
  const Lock L( mc_TaskLock );
  mv_nIdleTime = ac_nTimeInMilliSeconds;
}

void SlicedThread::mp_Run()
{
  unsigned nCalled = 0;
  unsigned nWaitTime = mv_nIdleTime;
  while( !IThread::mf_bThreadShouldStop() )
  {
    const Lock L( mc_TaskLock );

    mc_ListLock.mf_Enter();
    if( m_TaskList.mf_nGetNumItems() == 0 )
    {
      mv_nCurIndex = 0;
      mv_pCurTask = 0;
    }
    else
    {
      mv_pCurTask = m_TaskList.mf_GetItem( mv_nCurIndex );
    }

    if( mv_bNotify )
    {
      mv_bNotify = false;
      nCalled = 0;
    }
    mc_ListLock.mf_Leave();

    if( mv_pCurTask )
    {
      if( mv_pCurTask->mf_bWork() )
        nCalled = 0;
      else
        ++nCalled;
      if( ++mv_nCurIndex == m_TaskList.mf_nGetNumItems() )
        mv_nCurIndex = 0;
    }

    if( nCalled >= m_TaskList.mf_nGetNumItems() )
      nWaitTime = mv_nIdleTime;
    else if( mv_nCurIndex == 0 )
      nWaitTime = 5;
    else
      nWaitTime = 0;

    if( nWaitTime )
      IThread::mf_bWait( nWaitTime );
  }
}

