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
 
#include "thread.h"
#include "locks.h"
#include "containers/dynarray.h"

void* f_CreateThread( void* a_pUserData );
void  f_KillThread( void* a_hHandle );
void  f_SetThreadPriority( void* a_hHandle, const int ac_nPriority );
void  f_SetThreadCurrentName( const String& ac_sName );

using namespace appcore;
using namespace streamapp;

namespace
{
  DynamicArray<void*> s_RunningThreads;
  const CriticalSection s_RunningThreadsLock;
}

IThread::IThread( const String& ac_sName ) :
  mc_sName( ac_sName ),
  mv_hThreadHandle( 0 ),
  mv_nThreadID( 0 ),
  mv_ePriority( priority_normal ),
  mv_bShouldStop( false )
{
}

IThread::~IThread()
{
  mp_Stop( 100 );
}

#ifdef S_WIN32
void* IThread::sm_hAccurateSleep = 0;
#endif

void IThread::sf_ThreadEntryPoint( IThread* const ac_pThreadToRun )
{
  s_RunningThreadsLock.mf_Enter();
  s_RunningThreads.mp_AddItem( ac_pThreadToRun );
  s_RunningThreadsLock.mf_Leave();

  ac_pThreadToRun->mv_nThreadID = IThread::mf_nGetCurrentThreadID();

  if( ac_pThreadToRun->mc_StartWaiter.mf_eWaitForSignal( 10000 ) == WaitableObject::wait_ok )
  {
    ac_pThreadToRun->mp_Run();
  }

  s_RunningThreadsLock.mf_Enter();
  s_RunningThreads.mp_RemoveItem( ac_pThreadToRun );
  s_RunningThreadsLock.mf_Leave(); 
  
  ac_pThreadToRun->mv_hThreadHandle = 0;
  ac_pThreadToRun->mv_nThreadID = 0;
}

  /**
    * Used to wrap the call from the platform-specific code.
    */
void appcore::f_ThreadEntryPoint( void* a_pUserData )
{
  IThread::sf_ThreadEntryPoint( (IThread*) a_pUserData );
}

void IThread::mp_Start()
{
  const Lock L( mc_StartStopLock );
  if( mv_hThreadHandle == 0 )
  {
    mv_bShouldStop = false;
    mv_hThreadHandle = f_CreateThread( (void*) this );
    f_SetThreadPriority( mv_hThreadHandle, mv_ePriority );
    mc_StartWaiter.mp_SignalObject();
  }
}

void IThread::mp_Start( const mt_ePriority ac_ePriority )
{
  const Lock L( mc_StartStopLock );

  if( mv_hThreadHandle == 0 )
  {
    mv_ePriority = ac_ePriority;
    mp_Start();
  }
  else
  {
    mp_SetPriority( ac_ePriority );
  }
}

void IThread::mp_SetThreadShouldStop()
{
  mv_bShouldStop = true;
}

bool IThread::mf_bIsRunning() const
{
  return mv_hThreadHandle != 0;
}

bool IThread::mf_bWaitForThreadToStop( const int ac_nTimeoutInMSec ) const
{
    //how do you expect this thread to wait for itself to stop??
  assert( mf_nGetThreadID() != mf_nGetCurrentThreadID() );

    //sleep in pieces to allow checking regularly
  const int nMsecPerCount = 5;
  int nCounter = ac_nTimeoutInMSec / nMsecPerCount;

  while( mf_bIsRunning() )
  {
    if( ac_nTimeoutInMSec > 0 && --nCounter < 0 )
      return false;

    sf_Sleep( nMsecPerCount );
  }

  return true;
}

void IThread::mp_Stop( const int ac_nTimeoutInMSec )
{
  const Lock L( mc_StartStopLock );

  if( mf_bIsRunning() )
  {
    mp_SetThreadShouldStop();
    mf_NotifyWait();

    if( ac_nTimeoutInMSec !=0 )
      mf_bWaitForThreadToStop( ac_nTimeoutInMSec );

    if( mf_bIsRunning() )
    {
        //it's a Bad Evil Thing to reach this point,
        //so tell this to the user.
      //std::cout << "Thread " << mc_sName.c_str() << " killed without exiting thread function!" << std::endl;

      f_KillThread( mv_hThreadHandle );
      mv_hThreadHandle = 0;
      mv_nThreadID = 0;

      const Lock L2( s_RunningThreadsLock );
      s_RunningThreads.mp_RemoveItem( this );
    }
  }
}

void IThread::mp_SetPriority( const IThread::mt_ePriority ac_Priority )
{
  const Lock L( mc_StartStopLock );

  mv_ePriority = ac_Priority;
  f_SetThreadPriority( mv_hThreadHandle, ac_Priority );
}

void IThread::sf_SetCurrentThreadPriority( const IThread::mt_ePriority ac_Priority )
{
  f_SetThreadPriority( 0, ac_Priority );
}

bool IThread::mf_bWait( const int ac_nTimeoutInMSec ) const
{
  return mc_Waiter.mf_eWaitForSignal( ac_nTimeoutInMSec ) == WaitableObject::wait_ok;
}

void IThread::mf_NotifyWait() const
{
  mc_Waiter.mp_SignalObject();
}

unsigned IThread::sf_nGetNumRunningThreads()
{
  const Lock L( s_RunningThreadsLock );
  return s_RunningThreads.mf_nGetNumItems();
}

void IThread::sf_StopAllThreads( const int ac_nTimeoutInMSec )
{
  while( sf_nGetNumRunningThreads() > 0 )
  {
    s_RunningThreadsLock.mf_Enter();
    IThread* pStop = (IThread*) s_RunningThreads.mf_GetItem( 0 );
    s_RunningThreadsLock.mf_Leave();

    if( pStop )
      pStop->mp_Stop( ac_nTimeoutInMSec );
  }
}
