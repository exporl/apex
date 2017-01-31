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
 
#include "linux_headers.h"
#include <dlfcn.h>

#include "appcore/threads/thread.h"
#include "appcore/threads/waitableobject.h"
#include "appcore/threads/criticalsection.h"

using namespace appcore;

CriticalSection::CriticalSection()
{
  pthread_mutexattr_t atts;
  pthread_mutexattr_init( &atts );
  pthread_mutexattr_settype( &atts, PTHREAD_MUTEX_RECURSIVE );
  pthread_mutex_init( &m_hMutex, &atts );
}

CriticalSection::~CriticalSection()
{
  pthread_mutex_destroy( &m_hMutex );
}

void CriticalSection::mf_Enter() const
{
  pthread_mutex_lock( &m_hMutex );
}

bool CriticalSection::mf_bTryEnter() const
{
  return pthread_mutex_trylock( &m_hMutex ) == 0;
}

void CriticalSection::mf_Leave() const
{
  pthread_mutex_unlock( &m_hMutex );
}

/********************************************************************************************/

struct EventStruct
{
  pthread_cond_t  condition;
  pthread_mutex_t mutex;
  bool            triggered;
};

WaitableObject::WaitableObject()
{
  EventStruct* const es = new EventStruct();
  es->triggered = false;

  pthread_cond_init( &es->condition, 0 );
  pthread_mutex_init( &es->mutex, 0 );

  m_hHandle = es;
}

WaitableObject::~WaitableObject()
{
  EventStruct* const es = (EventStruct*) m_hHandle;

  pthread_cond_destroy (&es->condition);
  pthread_mutex_destroy (&es->mutex);

  delete es;
}

WaitableObject::mt_eWaitResult WaitableObject::mf_eWaitForSignal( const int ac_nTimeOut ) const
{
  EventStruct* const es = (EventStruct*) m_hHandle;

  bool ok = true;
  pthread_mutex_lock (&es->mutex);

  if (!es->triggered)
  {
    if (ac_nTimeOut < 0)
    {
      pthread_cond_wait (&es->condition, &es->mutex);
    }
    else
    {
      struct timespec time;
      struct timeval t;
      int timeout = 0;

      gettimeofday(&t,NULL);

      time.tv_sec  = t.tv_sec  + (ac_nTimeOut / 1000);
      time.tv_nsec = (t.tv_usec + ((ac_nTimeOut % 1000)*1000)) * 1000;
      while( time.tv_nsec >= 1000000000 )
      {
        time.tv_nsec -= 1000000000;
        time.tv_sec++;
      }

      while( !timeout )
      {
        timeout = pthread_cond_timedwait (&es->condition, &es->mutex, &time);
        if( !timeout )
          // Success
          break;

        if( timeout == EINTR )
          // Go round again
          timeout = 0;
      }
    }
    ok = es->triggered;
  }

  es->triggered = false;

  pthread_mutex_unlock (&es->mutex);
  return WaitableObject::wait_ok;
}

void WaitableObject::mp_SignalObject() const
{
  EventStruct* const es = (EventStruct*) m_hHandle;

  pthread_mutex_lock( &es->mutex );
  es->triggered = true;
  pthread_cond_signal( &es->condition );
  pthread_mutex_unlock( &es->mutex );
}

void WaitableObject::mp_ResetObject() const
{
  EventStruct* const eS = (EventStruct*) m_hHandle;

  pthread_mutex_lock( &eS->mutex );
  eS->triggered = false;
  pthread_mutex_unlock( &eS->mutex );
}

/********************************************************************************************/

void appcore::f_ThreadEntryPoint( void* a_pUserData );

void* threadEntryProc( void* a_pUserData )
{
    f_ThreadEntryPoint( a_pUserData );
    return 0;
}

void* f_CreateThread( void* a_pUserData )
{
  pthread_t handle = 0;

  if( pthread_create( &handle, 0, threadEntryProc, a_pUserData ) == 0 )
  {
    pthread_detach( handle );
    return (void*) handle;
  }

  return 0;
}

void  f_KillThread( void* a_hHandle )
{
  if( a_hHandle != 0 )
    pthread_cancel( (pthread_t) a_hHandle );
}

void  f_SetThreadPriority( void* /*a_hHandle*/, const int /*ac_nPriority*/ )
{
  //a bit complicated on linux.. needs pid which is hard to get + priority rules are weird
  //FIXMEEEEEE this is really needed, I have no idea what priority the IThreads run at now,
  //but it could as well be super-high.. face it, windows threads are better lol
}

void  f_SetThreadCurrentName( const std::string /*ac_sName*/ )
{

}

void IThread::sf_Sleep( const int ac_nMilliSeconds )
{
  struct timespec time;
  time.tv_sec = ac_nMilliSeconds / 1000;
  time.tv_nsec = ( ac_nMilliSeconds % 1000 ) * 1000000;
  nanosleep( &time, 0 );
}

int IThread::mf_nGetCurrentThreadID()
{
  return (int) pthread_self();
}
