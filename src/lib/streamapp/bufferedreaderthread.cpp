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
 
#include "bufferedreaderthread.h"
#include "bufferedreader.h"
#include "core/containers/circularlogic.h"

using appcore::Lock;
using appcore::IThread;
using namespace streamapp;

BufferedReaderThread::BufferedReaderThread() :
    IThread( "BufferedReaderThread" ),
  mv_nInterval( 15 ),
  m_Readers( 8, 2 )
{
  IThread::mp_Start( IThread::priority_normal );
}

BufferedReaderThread::~BufferedReaderThread()
{
  IThread::mp_Stop( 1000 );
}

void BufferedReaderThread::mp_Run()
{
  while( !IThread::mf_bThreadShouldStop() )
  {
    mc_Lock.mf_Enter();
    const unsigned& nReaders( m_Readers.mf_nGetNumItems() );
    if( !nReaders )
    {
      mc_Lock.mf_Leave();
      IThread::mf_bWait( 10000 );
    }
    else
    {
      for( unsigned i = 0 ; i < nReaders ; ++i )
        m_Readers.mf_GetItem( i )->mp_FillBuffer();
      mc_Lock.mf_Leave();
      IThread::mf_bWait( mv_nInterval );
    }
  }
}

void BufferedReaderThread::mp_AddReader( BufferedReader* a_pReader )
{
  const Lock L( mc_Lock );
  m_Readers.mp_AddItem( a_pReader );
  IThread::mf_NotifyWait();
}

void BufferedReaderThread::mp_RemoveReader( BufferedReader* a_pReader )
{
  const Lock L( mc_Lock );
  m_Readers.mp_RemoveItem( a_pReader );
  IThread::mf_NotifyWait();
}

void BufferedReaderThread::mp_SetTimeout( const int ac_nTimeout )
{
  const Lock L( mc_Lock );
  mv_nInterval = ac_nTimeout;
}
