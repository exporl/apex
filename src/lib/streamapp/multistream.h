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
 
#ifndef __MULTISTREAM_H__
#define __MULTISTREAM_H__

#include "stream.h"
#include "containers/dynstrlist.h"

namespace streamapp
{

    /**
      * MultiInputStream
      *   combines a number of InputStream objects into a single new InputStream.
      *   The number of channels of the resulting InputStream is the sum of the
      *   number of channels from all items added.
      *   BufferSize is always the buffersize of the first stream added;
      *   streams added afterwards must match this size.
      *   Always deletes items, so don't delete them yourself.
      *   @note adding an item results in calls to
      *   malloc et all, so it's wise to add everything before your app is streaming.
      *   @see InputStream
      *   @see MultiOutputStream
      ******************************************************************************* */
  template< bool tOwned = true >
  class MultiInputStream : public InputStream, public DynStreamList<InputStream, tOwned>
  {
  public:
      /**
        * Constructor.
        */
    MultiInputStream();

      /**
        * Destructor.
        * Calls mp_RemoveAllItems().
        * @see mp_RemoveAllItems()
        */
    ~MultiInputStream();

      /**
        * Implementation of the InputStream method.
        */
    const Stream& Read();

      /**
        * Implementation of the InputStream method.
        * Returns true when *all* added items' mf_bIsEndOfStream() returns true.
        */
    bool mf_bIsEndOfStream() const;

      /**
        * Implementation of the InputStream method.
        */
    INLINE unsigned mf_nGetNumChannels() const
    {
      return this->mf_nGetNumChannelsR();
    }

      /**
        * Implementation of the InputStream method.
        */
    INLINE unsigned mf_nGetBufferSize() const
    {
      return this->mf_nGetBufferSizeR();
    }

  protected:
      /**
        * Implementation of the DynStreamList method.
        */
    void mp_UpdateMemory();

    StreamChanPtr*  m_pRead;
    Stream*         m_pResult;

    MultiInputStream( const MultiInputStream& );
    MultiInputStream& operator = ( const MultiInputStream& );
  };

    /**
      * MultiPosInputStream
      *   same as MultiInputStream but positionable.
      *   In order to keep things simple,
      *   all inputs must have same length
      *   @see MultiInputStream
      *   @see PositionableInputStream
      ********************************************** */
  template< bool tOwned = true >
  class MultiPosInputStream : public PositionableInputStream, public DynStreamList<PositionableInputStream, tOwned>
  {
  public:
      /**
        * Constructor.
        */
    MultiPosInputStream();

      /**
        * Destructor.
        * Calls mp_RemoveAllItems().
        * @see mp_RemoveAllItems()
        */
    ~MultiPosInputStream();

      /**
        * Implementation of the InputStream method.
        */
    const Stream& Read();

      /**
        * Implementation of the DynStreamList method.
        * Override to check a_pItem is at the same play position as the rest.
        */
    void mp_AddItem( PositionableInputStream* a_pItem );

      /**
        * Implementation of the InputStream method.
        * Returns true when *all* added items' mf_bIsEndOfStream() returns true.
        */
    bool mf_bIsEndOfStream() const;

      /**
        * Implementation of the InputStream method.
        */
    INLINE unsigned mf_nGetNumChannels() const
    {
      return DynStreamList<PositionableInputStream>::mf_nGetNumChannelsR();
    }

      /**
        * Implementation of the InputStream method.
        */
    INLINE unsigned mf_nGetBufferSize() const
    {
      return this->mf_nGetBufferSizeR();
    }

      /**
        * Implementation of the PositionableInputStream method.
        */
    unsigned long mf_lSampleRate() const;

      /**
        * Implementation of the PositionableInputStream method.
        */
    unsigned long mf_lSamplesLeft() const;

      /**
        * Implementation of the PositionableInputStream method.
        */
    unsigned long mf_lTotalSamples() const;

      /**
        * Implementation of the PositionableInputStream method.
        */
    unsigned long mf_lCurrentPosition() const;

      /**
        * Implementation of the PositionableInputStream method.
        */
    void mp_SeekPosition( const unsigned long ac_nPosition );

      /**
        * Implementation of the PositionableInputStream method.
        */
    void mp_SetNumLoops( const unsigned ac_nLoops );

      /**
        * Implementation of the PositionableInputStream method.
        */
    unsigned mf_nGetNumLoops() const;

      /**
        * Implementation of the PositionableInputStream method.
        */
    unsigned mf_nGetNumLooped() const;

      /**
        * Implementation of the PositionableInputStream method.
        */
    bool mf_bIsLooping() const;

    INLINE bool mf_bCanLoop() const
    {
      return true;
    }

  protected:
      /**
        * Implementation of the DynStreamList method.
        */
    void mp_UpdateMemory();

    StreamChanPtr*      m_pRead;
    Stream*             m_pResult;

    MultiPosInputStream( const MultiPosInputStream& );
    MultiPosInputStream& operator = ( const MultiPosInputStream& );
  };


    /**
      * MultiOutputStream
      *   combines a number of OutputStream objects into a single new OutputStream.
      *   The number of channels of the resulting OutputStream is the sum of the
      *   number of channels from all items added, unless ac_bSerialMode is set.
      *   BufferSize is always the buffersize of the first stream added;
      *   streams added afterwards must match this size.
      *   Example:
      *   ac_bSerialMode = false:
      *   mp_AddOutputStream( 1 channel stream );
      *   mp_AddOutputStream( 2 channel stream );
      *   =>Final stream = 3 ( 1 + 2 ) channel, Write()'s input should be >=3 channels
      *   ac_bSerialMode = true (write one by one):
      *   mp_AddOutputStream( 1 channel stream );
      *   mp_AddOutputStream( 2 channel stream );
      *   =>Final stream = 2 channel, Write()'s input should be >=2 channels,
      *   first outputstream gets first channel, second one gets both of them
      ***************************************************************************** */
  template< bool tOwned = true >
  class MultiOutputStream : public OutputStream, public DynStreamList<OutputStream, tOwned>
  {
  public:
      /**
        * Constructor.
        * @param ac_bSerialMode if true, instead of
        */
    MultiOutputStream( const bool ac_bSerialMode = false );

      /**
        * Destructor.
        * Calls mp_RemoveAllItems().
        * @see mp_RemoveAllItems()
        */
    ~MultiOutputStream();

      /**
        * Implementation of the OutputStream method.
        */
    bool mf_bIsEndOfStream() const;

      /**
        * Implementation of the OutputStream method.
        */
    void Write(const Stream& ac_ToWrite );

      /**
        * Implementation of the InputStream method.
        */
    INLINE unsigned mf_nGetNumChannels() const
    {
      return this->mf_nGetNumChannelsR();
    }

      /**
        * Implementation of the InputStream method.
        */
    INLINE unsigned mf_nGetBufferSize() const
    {
      return this->mf_nGetBufferSizeR();
    }

  protected:
    const bool mc_bSerialMode;

    MultiOutputStream( const MultiOutputStream& );
    MultiOutputStream& operator = ( const MultiOutputStream& );
  };

}

#include "multistream.cpp"

#endif //#ifndef __MULTISTREAM_H__
