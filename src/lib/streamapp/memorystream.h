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

#ifndef __MEMORYSTREAM_H__
#define __MEMORYSTREAM_H__

#include "appcore/threads/criticalsection.h"

#include "audioformatconvertor.h"
#include "stream.h"

#include <QtGlobal>

namespace streamapp
{

    /**
      * MemoryInputStream
      *   reads stream from memory.
      *   This is much faster then from file since no disk access is required every call.
      *   TODO finish this
      *********************************************************************************** */
  class MemoryInputStream : public PositionableInputStream
  {
  public:
    MemoryInputStream( const Stream& ac_Memory, const unsigned ac_nBufferSize );
    ~MemoryInputStream();

      //!implement InputStream
    const Stream& Read();

    bool     mf_bIsEndOfStream() const
    { return mv_bEOF; }
    unsigned mf_nGetNumChannels() const
    { return mc_Memory.mf_nGetChannelCount(); }
    unsigned mf_nGetBufferSize() const
    { return mc_nBufferSize; }

      //!implement PositionableInputStream
    unsigned long mf_lSamplesLeft() const;
    unsigned long mf_lTotalSamples() const;
    unsigned long mf_lCurrentPosition() const;
    void mp_SeekPosition( const unsigned long ac_nPosition );

    void mp_SetNumLoops( const unsigned ac_nLoops );
    unsigned mf_nGetNumLoops() const;
    unsigned mf_nGetNumLooped() const;
    bool mf_bIsLooping() const;

  private:
    bool            mv_bEOF;
    const Stream&   mc_Memory;
    const unsigned  mc_nBufferSize;
    unsigned        mv_nLoops;
    unsigned        mv_nLooped;
    unsigned long   mv_lPosition;
    StreamPtrBuf    mv_Read;
    StreamBuf       mv_ReadEnd;
    bool            mv_bEndCleared;
    const appcore::CriticalSection mc_ReadLock;
  };

  template<class tType>
  struct ReverseInitter
  {
    static tType* mp_pSet( tType* ac_Ptr )
    {
      return ms_Ptr = ac_Ptr;
    }

    static tType* mf_pGet()
    {
      Q_ASSERT( ms_Ptr );
      return ms_Ptr;
    }

    static tType* ms_Ptr;
  };

  template<class tType>
  tType* ReverseInitter<tType>::ms_Ptr = 0;

  class AudioFormatMemoryInputStream : public MemoryInputStream
  {
  public:
    AudioFormatMemoryInputStream( PositionableAudioFormatReader* a_pReader, const unsigned ac_nBufferSize ) :
        MemoryInputStream( *ReverseInitter<AudioFormatConvertor>::mp_pSet( new AudioFormatConvertor( a_pReader->mf_nChannels(), a_pReader->mf_lTotalSamples() ) ), ac_nBufferSize ),
      mc_pSource( ReverseInitter<AudioFormatConvertor>::mf_pGet() )
    {
      mc_pSource->ReadFromSource( a_pReader, a_pReader->mf_lTotalSamples(), true );
    }

    ~AudioFormatMemoryInputStream()
    {
      delete mc_pSource;
    }

  private:
    AudioFormatConvertor* const mc_pSource;
  };


}

#endif //#ifndef __MEMORYSTREAM_H__
