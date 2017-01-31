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

#include "tracermemory.h"

#include <string.h>

#include <QtGlobal>

using namespace utils;

TracerMemory::TracerMemory( const unsigned ac_nInitSize /* = 1024 */, const unsigned ac_nMaxSize /* = 4096  */ ) :
  m_Trace( ac_nInitSize ),
  mv_bDebug( true ),
  mv_nWritePoint( 0 ),
  mc_nMaxSize( ac_nMaxSize )
{
  Q_ASSERT( ( ac_nMaxSize % 2 ) == 0 );
}

TracerMemory::~TracerMemory()
{
}

void TracerMemory::mp_TraceOne( const String& ac_sMessage )
{
  const Lock L( mc_Lock );

#ifdef S_C6X
  const char* pMess = ac_sMessage;
#else
  const char* pMess = ac_sMessage.data();
#endif

  const unsigned nLen = (unsigned) strlen( pMess ) + 1; //don't forget the terminator

  if( mv_nWritePoint + nLen < mc_nMaxSize )
  {
    m_Trace.mp_CopyFrom( pMess, mv_nWritePoint, nLen );
    mv_nWritePoint += nLen;
  }
  else
  {
    const unsigned nShift = mc_nMaxSize / 2;
    m_Trace.mp_ShiftDown( nShift, nShift );
    mv_nWritePoint = nShift;
    m_Trace.mp_CopyFrom( pMess, mv_nWritePoint, nLen );
    mv_nWritePoint += nLen;
  }
}
