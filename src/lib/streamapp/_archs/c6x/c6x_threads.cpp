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
 
#include "c6x_headers.h"
#include <stdlib.h>
#include <string.h>
#include "appcore/threads/thread.h"
#include "appcore/threads/waitableobject.h"
#include "appcore/threads/criticalsection.h"

using namespace appcore;

CriticalSection::CriticalSection()
{
  LCK_Attrs at;                  //FIXME we'd better have a macro here for allocating through DSP/Bios,
  m_hMutex = LCK_create( &at );  //this will save some memory
}

CriticalSection::~CriticalSection()
{
  LCK_delete( m_hMutex );
}

void CriticalSection::mf_Enter() const
{
  LCK_pend( m_hMutex, SYS_FOREVER );
}

bool CriticalSection::mf_bTryEnter() const
{
  if( LCK_pend( m_hMutex, 0 ) == TRUE )
    return true;
  return false;
}

void CriticalSection::mf_Leave() const
{
  LCK_post( m_hMutex );
}

/********************************************************************************************/

WaitableObject::WaitableObject()
{
  m_hHandle = SEM_create( 0, 0 );
}

WaitableObject::~WaitableObject()
{
  SEM_delete( (SEM_Handle) m_hHandle );
}

WaitableObject::mt_eWaitResult WaitableObject::mf_eWaitForSignal( const int ac_nTimeOut ) const
{
  Uns nTimeout = SYS_FOREVER;
  if( ac_nTimeOut != -1 )
    nTimeout = ac_nTimeOut;

  const Bool bRes = SEM_pend( (SEM_Handle) m_hHandle, nTimeout );

  if( bRes == TRUE )
    return wait_ok;
  else //WAIT_TIMEOUT
    return wait_timeout;
}

void WaitableObject::mp_SignalObject() const
{
  SEM_post( (SEM_Handle) m_hHandle );
}

void WaitableObject::mp_ResetObject() const
{
  SEM_reset( (SEM_Handle) m_hHandle, 0 );
}

/********************************************************************************************/

extern "C"
{
  static void vThreadEntryPoint( void* a_pUserData )
  {
    appcore::f_ThreadEntryPoint( a_pUserData );
         
      //Aaaauwwwch after a few hours of searching who was eating the precious memory,
      //it appears that (in contrary to what the docs say) a task's stack is NOT reused.
    	//Thus, better delete it here.
//    TSK_delete( TSK_self() );   
    
    TSK_exit();
    
    return;
	}   
}  

void f_KillThread( void* a_hHandle )
{
  if( a_hHandle != 0 )
    TSK_delete( (TSK_Handle) a_hHandle );
}

void* f_CreateThread( void* a_pUserData )
{
  //IThread* pThread = (IThread*) a_pUserData;

  struct TSK_Attrs ta;

  ta.priority  = 4;       //OS_TASKPRINORM
  ta.stack     = 0;
  ta.stacksize = 0x1000;  //a bit too big maybe?
  ta.stackseg  = 0;
  ta.environ   = 0;
  //int nLen = strlen( pThread->mf_sGetName() );
  ta.name = 0;//(String) malloc( nLen + 1 );  //ends with null
  //strcpy( ta.name, pThread->mf_sGetName() );
  ta.exitflag  = 0;

  return TSK_create( (Fxn)vThreadEntryPoint, &ta, a_pUserData );
}

void f_SetThreadPriority( void* a_hHandle, const int ac_nPriority )
{
  int pri = 4;  //norm

  if( ac_nPriority < 1 )
    pri = -1;    //this will completely stop execution.
  else if( ac_nPriority < 2 )
    pri = 1;
  else if( ac_nPriority < 5 )
    pri = 3;
  else if( ac_nPriority < 7 )
    pri = 4;
  else if( ac_nPriority < 9 )
    pri = 8;
  else if( ac_nPriority < 10 )
    pri = 10;

  if( a_hHandle == 0 )
    a_hHandle = TSK_self();

  TSK_setpri( (TSK_Handle) a_hHandle, pri );
}

int IThread::mf_nGetCurrentThreadID()
{
  return (int) TSK_self();	//no id here, so use the thread handle
}

void IThread::sf_Sleep( const int ac_nMilliSeconds )
{
  TSK_sleep( ac_nMilliSeconds );
}

