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

  /**
    * @file streamutils.h
    * Utility classes used by streamapp classes.
    * Most of them do tricky pointer stuff,
    * only use these if you know what you're doing.
    */

#ifndef _APEX_SRC_LIB_STREAMAPP_STREAMUTILS_H__
#define _APEX_SRC_LIB_STREAMAPP_STREAMUTILS_H__

#include "containers/matrix.h"

#include "stream.h"

#include <QtGlobal>

namespace streamapp
{

    /**
      * NullStream
      *   an InputStream reading only zeros.
      ************************************** */
  class NullStream : public InputStream
  {
  public:
      /**
        * Constructor.
        * @param ac_nBufferSize the buffersize
        * @param ac_nChannels the number of channels
        */
    NullStream( const unsigned ac_nBufferSize, const unsigned ac_nChannels ) :
      m_Null( ac_nChannels, ac_nBufferSize, true )
    {
    }

      /**
        * Implementation of the InputStream method.
        */
    INLINE const Stream& Read()
    {
      return m_Null;
    }

      /**
        * Implementation of the InputStream method.
        */
    INLINE unsigned mf_nGetNumChannels() const
    {
      return m_Null.mf_nGetChannelCount();
    }

      /**
        * Implementation of the InputStream method.
        */
    INLINE unsigned mf_nGetBufferSize() const
    {
      return m_Null.mf_nGetBufferSize();
    }

      /**
        * Implementation of the InputStream method.
        */
    INLINE bool mf_bIsEndOfStream() const
    {
      return false;
    }

      /**
        * Implementation of the InputStream method.
        */
    INLINE unsigned mf_nGetNumSamplesRead() const
    {
      return mf_nGetBufferSize();
    }

  private:
    StreamBuf m_Null;    //FIXME single buffer should do
  };

    /**
      * PtrStream
      *   InputStream made by combining pointers to channels from multiple streams.
      *   All pointers initially point to a NullChannel thus instances can be used
      *   as is right after construction.
      *   Never deletes items.
      *   Use *very* carefully.
      **************************************************************************** */
  class PtrStream : public InputStream
  {
  public:
      /**
        * Constructor.
        * @param ac_nBufferSize the buffersize
        * @param ac_nChannels the number of channels
        */
    PtrStream( const unsigned ac_nBufferSize, const unsigned ac_nChannels ) :
      m_Ptr( ac_nChannels ),
      m_Nulls( ac_nBufferSize, true ),
      m_Str( m_Ptr.mf_pGetArray(), ac_nChannels, ac_nBufferSize )
    {
      for( unsigned i = 0 ; i < ac_nChannels ; ++i )
        SetChannel( i, m_Nulls.mf_pGetArray() );
    }

      /**
        * Destructor.
        */
    ~PtrStream()
    {
    }

      /**
        * Set the pointer for the specified channel.
        * Better make sure a_pChannel points to something valid.
        * @param ac_nIndex the channel
        * @param a_pChannel the pointer to memory
        */
    INLINE void SetChannel( const unsigned ac_nIndex, StreamType* a_pChannel )
    {
      m_Ptr.mp_Set( ac_nIndex, a_pChannel );
    }

      /**
        * Get the pointer for the specified channel.
        * @param ac_nIndex the channel
        * @return the pointer
        */
    INLINE const StreamType* GetChannel( const unsigned ac_nIndex ) const
    {
      return m_Ptr( ac_nIndex );
    }

      /**
        * Set the specified channel to the null pointer.
        * @param ac_nIndex the channel
        */
    INLINE void ResetChannel( const unsigned ac_nIndex )
    {
      m_Ptr.mp_Set( ac_nIndex, m_Nulls.mf_pGetArray() );
    }

      /**
        * Check if the channel at the specified index points to the zeros.
        * @param ac_nIndex the channel
        * @return true if it's a null channel
        */
    INLINE bool mf_bIsNullChannel( const unsigned ac_nIndex ) const
    {
      return m_Ptr( ac_nIndex ) == m_Nulls.mf_pGetArray();
    }

      /**
        * Get the first channel pointing to the zeros.
        * @return the channel, or -1 if none free
        */
    INLINE int mf_nGetFirstNullChannel() const
    {
      const unsigned nChan = m_Ptr.mf_nGetBufferSize();
      for( unsigned i = 0 ; i < nChan ; ++i )
      {
        if( mf_bIsNullChannel( i ) )
          return (int) i;
      }
      return -1;
    }

      /**
        * Get the number of channles pointing to zeros.
        * @return the number
        */
    INLINE unsigned mf_nGetNumNullChannels() const
    {
      unsigned nRet = 0;
      const unsigned nChan = m_Ptr.mf_nGetBufferSize();
      for( unsigned i = 0 ; i < nChan ; ++i )
        if( mf_bIsNullChannel( i ) )
          ++nRet;
      return nRet;
    }

      /**
        * Implementation of the InputStream method.
        */
    INLINE const Stream& Read()
    {
      return m_Str;
    }

      /**
        * Implementation of the InputStream method.
        */
    INLINE unsigned mf_nGetNumChannels() const
    {
      return m_Str.mf_nGetChannelCount();
    }

      /**
        * Implementation of the InputStream method.
        */
    INLINE unsigned mf_nGetBufferSize() const
    {
      return m_Str.mf_nGetBufferSize();
    }

      /**
        * Implementation of the InputStream method.
        */
    INLINE bool mf_bIsEndOfStream() const
    {
      return false;
    }

  private:
    StreamChanPtr         m_Ptr;
    ArrayStorage<StreamType>  m_Nulls;
    const Stream          m_Str;

    PtrStream( const PtrStream& );
    PtrStream& operator = ( const PtrStream& );
  };

    /**
      * StreamChannels
      *   a Stream with channels made by copying the
      *   required pointers from the input.
      *   Use this to split a Stream into it's channels.
      ************************************************** */
  class StreamChannels : public Stream
  {
  public:
      /**
        * Constructor.
        * @param ac_Input the Stream containing the channel to get.
        * @param ac_nChannelToGet the channel to get
        */
    StreamChannels( const Stream& ac_Input, const unsigned ac_nChannelToGet ) :
      Stream( new StreamType*[ 1 ], 1, ac_Input.mf_nGetBufferSize() )
    {
      Q_ASSERT( ac_nChannelToGet < ac_Input.mf_nGetChannelCount() );
      Stream::mc_pSamplesArray[ 0 ] = ac_Input.mf_pGetArray()[ ac_nChannelToGet ];
    }

      /**
        * Destructor.
        */
    virtual ~StreamChannels()
    {
      delete [] mc_pSamplesArray;
    }

  private:
    StreamChannels( const StreamChannels& );
    StreamChannels& operator = ( const StreamChannels& );
  };

}

#endif //#ifndef _APEX_SRC_LIB_STREAMAPP_STREAMUTILS_H__
