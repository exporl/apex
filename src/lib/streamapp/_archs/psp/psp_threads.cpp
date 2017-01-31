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

#include <pspkernel.h>

using namespace appcore;

CriticalSection::CriticalSection()
{
  m_hMutex = 0;
}

CriticalSection::~CriticalSection()
{
}

void CriticalSection::mf_Enter() const
{
}

bool CriticalSection::mf_bTryEnter() const
{
  return true;
}

void CriticalSection::mf_Leave() const
{
}

/********************************************************************************************/

WaitableObject::WaitableObject()
{
  m_hHandle = 0;
}

WaitableObject::~WaitableObject()
{

}

WaitableObject::mt_eWaitResult WaitableObject::mf_eWaitForSignal( const int ac_nTimeOut ) const
{
  return wait_ok;
}

void WaitableObject::mp_SignalObject() const
{

}

void WaitableObject::mp_ResetObject() const
{

}

/********************************************************************************************/

int vThreadEntryPoint( SceSize args, void* argp )
{
  appcore::f_ThreadEntryPoint( argp );
  sceKernelTerminateThread( sceKernelGetThreadId() );
  return 0;
}

void* f_CreateThread( void* a_pUserData )
{
    //!!most libs require user mode (vs kernel mode) so we start this way
  IThread* pThread = (IThread*) a_pUserData;
  int thid = sceKernelCreateThread( pThread->mf_sGetName().data(), vThreadEntryPoint, 0x18, 0x10000, PSP_THREAD_ATTR_USER, 0 );

  if( thid >= 0 )
    sceKernelStartThread( thid, sizeof( void* ), a_pUserData );

  return (void*) thid;
}

void  f_KillThread( void* a_hHandle )
{
  if( a_hHandle != 0 )
    sceKernelDeleteThread( (int) a_hHandle );
}

void f_SetThreadPriority( void* a_hHandle, const int ac_nPriority )
{
  int pri = 0;                                                      //CHECKME what if two threads have same priority?

  if( ac_nPriority < 1 )
    pri = 0x16;
  else if( ac_nPriority < 2 )
    pri = 0x10;
  else if( ac_nPriority < 5 )
    pri = 0x08;
  else if( ac_nPriority < 7 )
    pri = 0x06;
  else if( ac_nPriority < 9 )
    pri = 0x04;
  else if( ac_nPriority < 10 )
    pri = 0x02;

  if( a_hHandle == 0 )
    a_hHandle = (void*) sceKernelGetThreadId();

  sceKernelChangeThreadPriority( (int) a_hHandle, pri );
}

void  f_SetThreadCurrentName( const std::string ac_sName )
{
  //psp is one of the few that can set thread names, but only when creating it (see f_CreateThread)
}

int IThread::mf_nGetCurrentThreadID()
{
  return sceKernelGetThreadId();
}

void IThread::sf_Sleep( const int ac_nMilliSeconds )
{
  sceKernelDelayThread( (SceUInt) ( ac_nMilliSeconds * 1000 ) );       //delay is in uSec
}
