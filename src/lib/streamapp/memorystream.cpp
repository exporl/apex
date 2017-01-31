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
 
#include "memorystream.h"
#include "appcore/threads/locks.h"

using namespace appcore;
using namespace streamapp;

MemoryInputStream::MemoryInputStream( const Stream& ac_Memory, const unsigned ac_nBufferSize ) :
  mv_bEOF( false ),
  mc_Memory( ac_Memory ),
  mc_nBufferSize( ac_nBufferSize ),
  mv_nLoops( 1 ),
  mv_nLooped( 0 ),
  mv_lPosition( 0L ),
  mv_Read( ac_Memory.mf_nGetChannelCount(), ac_nBufferSize ),
  mv_ReadEnd( ac_Memory.mf_nGetChannelCount(), ac_nBufferSize ),
  mv_bEndCleared( false ),
  mc_ReadLock()
{

}

MemoryInputStream::~MemoryInputStream()
{
  mv_Read.mf_pGetArray()[ 0 ] = 0;
}

const Stream& MemoryInputStream::Read()
{
  const Lock L( mc_ReadLock );

    //check EOF
  if( mv_bEOF )
  {
    if( !mv_bEndCleared )
    {
      mv_ReadEnd.mp_Clear();
      mv_bEndCleared = true;
    }
    return mv_Read;
  }

  throw( 0 && "not implemented" );
}

INLINE unsigned long MemoryInputStream::mf_lSamplesLeft() const
{
  return mf_lTotalSamples() - mf_lCurrentPosition();
}

INLINE unsigned long MemoryInputStream::mf_lCurrentPosition() const
{
  if( mf_bIsLooping() )
    return ( mv_nLooped * mc_Memory.mf_nGetBufferSize() ) + mv_lPosition;
  return mv_lPosition;
}

INLINE unsigned long MemoryInputStream::mf_lTotalSamples() const
{
  return mv_nLoops * mc_Memory.mf_nGetBufferSize();
}

void MemoryInputStream::mp_SeekPosition( const unsigned long ac_nPosition )
{
  assert( ac_nPosition <= mf_lTotalSamples() * mf_nGetNumLoops() );

  const Lock L( mc_ReadLock );

  if( mv_nLoops == 1 )
  {
    mv_lPosition = ac_nPosition;
    mv_nLooped = 0;
    mv_bEOF = false;
    mv_bEndCleared = false;
  }
  else
  {
    const unsigned long nTotal = mf_lTotalSamples();
    const unsigned long nStrip = ac_nPosition % nTotal;
    const unsigned      nFMode = ac_nPosition / nTotal;
    mv_lPosition = nStrip;
    mv_bEOF = false;
    mv_bEndCleared = false;
    mv_nLooped = nFMode;
  }
}

INLINE void MemoryInputStream::mp_SetNumLoops( const unsigned ac_nLoops )
{
  const Lock L( mc_ReadLock );
  if( !mv_bEOF && ac_nLoops >= mv_nLoops )
    mv_nLoops = ac_nLoops;
  else
    assert( 0 && "loop not set" );
}

INLINE unsigned MemoryInputStream::mf_nGetNumLoops() const
{
  return mv_nLoops;
}

INLINE unsigned MemoryInputStream::mf_nGetNumLooped() const
{
  return mv_nLooped;
}

INLINE bool MemoryInputStream::mf_bIsLooping() const
{
  return mv_nLoops != 1;
}
