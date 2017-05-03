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

#ifndef _APEX_SRC_LIB_STREAMAPP_AUDIOFORMAT_H_
#define _APEX_SRC_LIB_STREAMAPP_AUDIOFORMAT_H_

#include <string>

namespace streamapp
{
    /**
      * Audioformat
      *   interface describing an audio format.
      *   TODO extend a bit
      ***************************************** */
  class AudioFormat
  {
  protected:
      /**
        * Potected Constructor.
        */
    AudioFormat()
    {
    }

  public:
      /**
        * Destructor.
        */
    virtual ~AudioFormat()
    {
    }

#ifdef S_C6X
      /**
        * Bitmodes for our C6X ADC
        */
    enum AudioFormat::mt_eBitMode
    {
      MSBshort12,
      MSBshort14,
      MSBshort16
    };
#else
      /**
        * Bitmodes for audio samples
        */
    enum mt_eBitMode
    {
      MSBint16,
      MSBint16debug,
      MSBint24,
      MSBint32,
      MSBfloat32,
      MSBfloat64
    }; //FIXME need to check endianess too!!
#endif

      /**
        * Get the number of channels of the format.
        * @return the number of channels
        */
    virtual unsigned mf_nChannels() const = 0;

      /**
        * Get the samplerate of the format.
        * @return the samplerate
        */
    virtual unsigned long mf_lSampleRate() const = 0;

      /**
        * Get the bitmode of the format.
        * @return one of AudioFormat::mt_eBitMode
        */
    virtual AudioFormat::mt_eBitMode mf_eBitMode() const = 0;
  };


    /**
      * AudioFormatReader
      *   interface for anything that can read audio samples.
      *   The actual format must be one of AudioFormat::mt_eBitMode. Formats with
      *   less then 32 bits are required to shift the samples
      *   to 32bit first (it's faster to do this in one go).
      ************************************************************ */
  class AudioFormatReader : public AudioFormat
  {
  protected:
      /**
        * Potected Constructor.
        */
    AudioFormatReader()
    {}
  public:
      /**
        * Destructor.
        */
    virtual ~AudioFormatReader()
    {}

      /**
        * Read mf_nChannels() * ac_nSamples samples.
        * For bitmodes with 32 or less bits, a_pBuf is int**,
        * else it is double**.
        * @param a_pBuf the memory to place the samples in
        * @param ac_nSamples the number of channels to read, per channel
        * @return the actual number of samples read into a_pBuf
        */
    virtual unsigned long Read( void** a_pBuf, const unsigned ac_nSamples ) = 0;
  };

    /**
      * AudioFormatWriter
      *   interface for anything that can write audio samples.
      *   The actual format must be one of AudioFormat::mt_eBitMode.
      ******************************************************** */
  class AudioFormatWriter : public AudioFormat
  {
  protected:
      /**
        * Potected Constructor.
        */
    AudioFormatWriter()
    {}
  public:
      /**
        * Destructor.
        */
    virtual ~AudioFormatWriter()
    {}

      /**
        * Wite mf_nChannels() * ac_nSamples.
        * For bitmodes with 32 or less bits, a_pBuf is int**,
        * and contains 32bit int of float depending on the format.
        * Else it is double**.
        * @param a_pBuf the memory containing the samples
        * @param ac_nSamples the number of channels in a_pBuf, per channel
        * @return the actual number of samples written
        */
    virtual unsigned long Write( const void** a_pBuf, const unsigned ac_nSamples ) = 0;
  };


    /**
      * PositionableAudioFormatReader
      *   base class for finite readers that can be positioned.
      *   AudioFormatReader just reads samples without knowing
      *   about current time or length, so define an interface
      *   that is somewhat smarter by specifying the total number
      *   of samples and the read position.
      *   @see AudioFormatReader
      *********************************************************** */
  class PositionableAudioFormatReader : public AudioFormatReader
  {
  protected:
      /**
        * Potected Constructor.
        */
    PositionableAudioFormatReader() :
      mv_lLoopStart( 0L ),
      mv_lLoopEnd( 0L )
    {
    }

    unsigned long mv_lLoopStart;
    unsigned long mv_lLoopEnd;

  public:
      /**
        * Destructor.
        */
    virtual ~PositionableAudioFormatReader()
    {
    }

      /**
        * Get the number of samples left for reading.
        * @return the number of samples
        */
    virtual unsigned long mf_lSamplesLeft() const = 0;

      /**
        * Get the total number of samples in the reader.
        * @return total number of samples
        */
    virtual unsigned long mf_lTotalSamples() const = 0;

      /**
        * Get the position of next sample going to be read when Read() is called.
        * @return the position
        */
    virtual unsigned long mf_lCurrentPosition() const = 0;

      /**
        * Set the position.
        * Sets the sample that is going to be read on the next Read() call.
        * @param ac_lPosition the next sample
        */
    virtual void mp_SeekPosition( const unsigned long ac_lPosition ) = 0;

      /**
        * Stat the start point for looping.
        * @param the first loop point
        * @return false if out of range or if the source
        * doesn't support looping
        */
    virtual bool mp_bSetLoopStart( const unsigned long ac_lPosition )
    {
      if( ac_lPosition > mf_lTotalSamples() )
      {
        mv_lLoopStart = mv_lLoopEnd;
        return false;
      }
      mv_lLoopStart = ac_lPosition;
      return true;
    }

      /**
        * Set the end point for looping.
        * @param the second loop point
        * @return false if out of range or if the source
        * doesn't support looping
        */
    virtual bool mp_bSetLoopEnd( const unsigned long ac_lPosition )
    {
      if( ac_lPosition > mf_lTotalSamples() )
      {
        mv_lLoopEnd = mv_lLoopStart;
        return false;
      }
      mv_lLoopEnd = ac_lPosition;
      return true;
    }

    virtual void mp_SetLoopAll()
    {
      mv_lLoopStart = 0;
      mv_lLoopEnd = mf_lTotalSamples();
    }

      /**
        *
        */
    virtual bool mf_bIsLooping() const
    {
      return mv_lLoopEnd != mv_lLoopStart;
    }
  };

}

#endif //#ifndef _APEX_SRC_LIB_STREAMAPP_AUDIOFORMAT_H_
