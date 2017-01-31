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
 
#include "appcore/threads/thread.h"
#include "appcore/threads/waitableobject.h"
#include "appcore/threads/criticalsection.h"
#include "win32_headers.h"
#include <assert.h>
#include <process.h>

using namespace appcore;

CriticalSection::CriticalSection() 
{
    //check MS haven't changed this structure
  assert( sizeof (CRITICAL_SECTION) <= sizeof( m_hMutex ) );

  InitializeCriticalSection( (CRITICAL_SECTION*) m_hMutex );
}

CriticalSection::~CriticalSection() 
{
  DeleteCriticalSection( (CRITICAL_SECTION*) m_hMutex );
}

void CriticalSection::mf_Enter() const 
{
  EnterCriticalSection( (CRITICAL_SECTION*) m_hMutex );
}

bool CriticalSection::mf_bTryEnter() const 
{
  return TryEnterCriticalSection( (CRITICAL_SECTION*) m_hMutex) != FALSE;
}

void CriticalSection::mf_Leave() const 
{
  LeaveCriticalSection( (CRITICAL_SECTION*) m_hMutex );
}

/********************************************************************************************/

WaitableObject::WaitableObject() :
  m_hHandle( CreateEvent( 0, 0, 0, 0 ) )
{
}

WaitableObject::~WaitableObject() 
{
  CloseHandle( m_hHandle );
}

WaitableObject::mt_eWaitResult WaitableObject::mf_eWaitForSignal( const int ac_nTimeOut ) const 
{
  const DWORD dwRes = WaitForSingleObject( m_hHandle, ac_nTimeOut );
  if( dwRes == WAIT_OBJECT_0 )
    return wait_ok;
  else if( dwRes == WAIT_ABANDONED )
    return wait_failed;
  else //WAIT_TIMEOUT
    return wait_timeout;
}

void WaitableObject::mp_SignalObject() const 
{
  SetEvent( m_hHandle );
}

void WaitableObject::mp_ResetObject() const 
{
  ResetEvent( m_hHandle );
}

/********************************************************************************************/

void appcore::f_ThreadEntryPoint( void* a_pUserData );

static unsigned __stdcall stdf_nThreadEntryPoint( void* a_pUserData )
{
  appcore::f_ThreadEntryPoint( a_pUserData );
  _endthread();
  return 0;
}

namespace
{
  HANDLE    s_SleepEvent = 0;
  unsigned  s_nTreads    = 0; //FIXME make thread pool manager or so

  void f_InitialiseThreadEvents()
  {
    ++s_nTreads;
    if( IThread::sm_hAccurateSleep == 0 )
      IThread::sm_hAccurateSleep = CreateEvent( 0, 0, 0, 0 );
  }

  void f_DestroyThreadEvents()
  {
    --s_nTreads;
    if( s_nTreads == 0 )
    {
      if( IThread::sm_hAccurateSleep != 0 )
      {
        CloseHandle( IThread::sm_hAccurateSleep );
        IThread::sm_hAccurateSleep = 0;
      }
    }
  }
}

void* f_CreateThread( void* a_pUserData )
{
  unsigned int threadId = 0;
  f_InitialiseThreadEvents();
  return (void*) _beginthreadex( 0, 0, &stdf_nThreadEntryPoint, a_pUserData, 0, &threadId );
}

void  f_KillThread( void* a_hHandle )
{
  if( a_hHandle != 0 )
    TerminateThread( a_hHandle, 0 );
  f_DestroyThreadEvents();
}

void  f_SetThreadPriority( void* a_hHandle, const int ac_nPriority )
{
  int pri = THREAD_PRIORITY_TIME_CRITICAL;

  if( ac_nPriority < 1 )
    pri = THREAD_PRIORITY_IDLE;
  else if( ac_nPriority < 2 )
    pri = THREAD_PRIORITY_LOWEST;
  else if( ac_nPriority < 5 )
    pri = THREAD_PRIORITY_BELOW_NORMAL;
  else if( ac_nPriority < 7 )
    pri = THREAD_PRIORITY_NORMAL;
  else if( ac_nPriority < 9 )
    pri = THREAD_PRIORITY_ABOVE_NORMAL;
  else if( ac_nPriority < 10 )
    pri = THREAD_PRIORITY_HIGHEST;

  if( a_hHandle == 0 )
    a_hHandle = GetCurrentThread();

  SetThreadPriority( a_hHandle, pri );
}

void f_SetThreadCurrentName( const std::string /*ac_sName*/ )
{
}

void IThread::sf_Sleep( const int ac_nMilliSeconds ) 
{
  if( ac_nMilliSeconds >= 10 )
  {
    Sleep( ac_nMilliSeconds );
  }
  else
  {
    assert( IThread::sm_hAccurateSleep );

      //unlike Sleep() this is guaranteed to return to the current thread after
      //the time expires, so we'll use this for short waits, which are more likely
      //to need to be accurate
    WaitForSingleObject( IThread::sm_hAccurateSleep, ac_nMilliSeconds );
  }
}

int IThread::mf_nGetCurrentThreadID()
{
  return (int) GetCurrentThreadId();
}
