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

#ifndef _APEX_SRC_LIB_STREAMAPP_AUDIOFORMATSTREAM_H__
#define _APEX_SRC_LIB_STREAMAPP_AUDIOFORMATSTREAM_H__

#include "appcore/threads/criticalsection.h"
#include "audioformat.h"
#include "audioformatconvertor.h"
#include "defines.h"
#include "stream.h"

namespace streamapp
{

/**
  * AudioFormatReaderStream
  *   used to transform an AudioFormatReader into an InputStream.
  *   Every Read() call reads ac_nBufferSize samples from the source,
  *   using an AudioFormatConvertor to get StreamType samples.
  *   When the source is exhausted, and it returns less new samples
  *   then the buffersize, zeros are added for the remaining samples,
  *   and by the next Read() call, the entire output is cleared.
  ******************************************************************* */
class AudioFormatReaderStream : public InputStream
{
public:
    /**
      * Constructor.
      * @param ac_cpSource the AudioFormatReader to read from (also defines the
     * number of channels)
      * @param ac_nBufferSize the buffer size in samples
      * @param ac_bDeleteReader if true, deletes ac_pSource upon destruction or
     * replacing reader
      */
    AudioFormatReaderStream(AudioFormatReader *const ac_cpSource,
                            const unsigned ac_nBufferSize,
                            const bool ac_bDeleteReader);

    /**
      * Destructor.
      * Deletes reader if required.
      */
    virtual ~AudioFormatReaderStream();

    /**
      * Implementation of the InputStream method.
      */
    INLINE bool mf_bIsEndOfStream() const
    {
        return mv_bEOF;
    }

    /**
      * Implementation of the InputStream method.
      */
    INLINE unsigned mf_nGetNumChannels() const
    {
        return m_Output.mf_nGetChannelCount();
    }

    /**
      * Implementation of the InputStream method.
      */
    INLINE unsigned mf_nGetBufferSize() const
    {
        return m_Output.mf_nGetBufferSize();
    }

    /**
      * Implementation of the InputStream method.
      */
    INLINE unsigned mf_nGetNumSamplesRead() const
    {
        return mv_nReadLastTime;
    }

    /**
      * Implementation of the InputStream method.
      */
    virtual const Stream &Read();

    /**
      * Replace the reader.
      * The new reader must have same bitmode and number of channels.
      * @param ac_pSource the new AudioFormatReader
      * @param ac_bDeleteReader if true, deletes ac_pSource upon destruction or
     * replacing reader
      */
    void mp_ReplaceReader(AudioFormatReader *const ac_pSource,
                          const bool ac_bDeleteReader = false);

    /**
      * Get the current source.
      * @return the reader
      */
    INLINE const AudioFormatReader *mf_pGetReader() const
    {
        return m_pSource;
    }

    /**
      * Get the AudioFormatConvertor.
      * Can be used to do low-level stuff directly.
      * @return an AudioFormatConvertor reference
      */
    INLINE AudioFormatConvertor &mp_GetConvertor()
    {
        return m_Output;
    }

    /**
      * Set the gain to apply.
      * @see AudioFormatConvertor::mp_SetGain()
      * @param the gain, linear scale
      */
    void mp_SetGain(const double ac_dGain);

    /**
      * Get the current gain.
      * @return the gain
      */
    const double &mf_dGetGain() const;

private:
    AudioFormatReader *m_pSource;
    AudioFormatConvertor m_Output;
    unsigned mv_nReadLastTime;
    bool mv_bDeleteReader;
    bool mv_bEOF;
    bool mv_bOutputCleared;
    const appcore::CriticalSection mc_ReadLock;
};

/**
  * PositionableAudioFormatReaderStream
  *   used to transform a PositionableAudioFormatReader into a
  *PositionableInputStream.
  *   Every Read() call reads ac_nBufferSize samples from the source, when the
  *source
  *   is exhausted, zeros are added, unless the number of loops is > 1.
  *   When looping, and the end of the reader's position is reached, the read
  *position
  *   is reset to 0 and everything starts all over again, except when number of
  *loops
  *   becomes equal to the number of loops set by mp_SetNumLoops: then zeroes
  *are added
  *   and the EOF flag is set.
  *   @see InputStream
  *************************************************************************************
  */
class PositionableAudioFormatReaderStream : public PositionableInputStream
{
public:
    /**
      * Constructor.
      * @param ac_pSource the PositionableAudioFormatReader to read from
     * (defines number of channels)
      * @param ac_nBufferSize the buffersize in samples
      * @param ac_bDeleteReader if true, deletes ac_pSource upon destruction or
     * replacing reader
      */
    PositionableAudioFormatReaderStream(
        PositionableAudioFormatReader *const ac_pSource,
        const unsigned ac_nBufferSize, const bool ac_bDeleteReader);

    /**
      * Destructor.
      * Deletes reader if required.
      */
    ~PositionableAudioFormatReaderStream();

    /**
      * Implementation of the InputStream method.
      */
    const Stream &Read();

    /**
      * Implementation of the InputStream method.
      */
    INLINE bool mf_bIsEndOfStream() const
    {
        return mv_bEOF;
    }

    /**
      * Implementation of the InputStream method.
      */
    INLINE unsigned mf_nGetNumChannels() const
    {
        return m_Output.mf_nGetChannelCount();
    }

    /**
      * Implementation of the InputStream method.
      */
    INLINE unsigned mf_nGetBufferSize() const
    {
        return m_Output.mf_nGetBufferSize();
    }

    /**
      * Implementation of the InputStream method.
      */
    INLINE unsigned mf_nGetNumSamplesRead() const
    {
        return mv_nReadLastTime;
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
    void mp_SeekPosition(const unsigned long ac_nPosition);

    /**
      * Implementation of the PositionableInputStream method.
      */
    INLINE bool mf_bCanLoop() const
    {
        return true;
    }

    /**
      * Implementation of the PositionableInputStream method.
      */
    void mp_SetNumLoops(const unsigned ac_nLoops);

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

    /**
      * @see AudioFormatReaderStream
      */
    void mp_ReplaceReader(PositionableAudioFormatReader *const ac_pSource,
                          const bool ac_bDeleteReader);

    /**
      * @see AudioFormatReaderStream
      */
    INLINE PositionableAudioFormatReader *mf_pGetReader() const
    {
        return m_pSource;
    }

    /**
      * Set the gain to apply.
      * @see AudioFormatConvertor::mp_SetGain()
      * @param the gain, linear scale
      */
    void mp_SetGain(const double ac_dGain);

    /**
      * Get the current gain.
      * @return the gain
      */
    const double &mf_dGetGain() const;

    /**
      * Get the AudioFormatConvertor.
      * Can be used to do low-level stuff directly.
      * @return an AudioFormatConvertor reference
      */
    INLINE AudioFormatConvertor &mp_GetConvertor()
    {
        return m_Output;
    }

private:
    bool mv_bDeleteReader;
    bool mv_bEOF;
    bool mv_bOutputCleared;
    unsigned mv_nReadLastTime;
    unsigned mv_nLoops;
    unsigned mv_nLooped;
    AudioFormatConvertor m_Output;
    PositionableAudioFormatReader *m_pSource;
    const appcore::CriticalSection mc_ReadLock;
};

/**
  * Use this typedef if you don't like long names.
  */
typedef PositionableAudioFormatReaderStream PosAudioFormatStream;

/**
  * AudioFormatWriterStream
  *   used to transform an AudioFormatWriter into an OutputStream.
  *   Every Write() call writes ac_nBufferSize samples to the writer.
  ******************************************************************* */
class AudioFormatWriterStream : public OutputStream
{
public:
    /**
      * Constructor.
      * @param ac_pSink the AudioFormatWriter to write to (defines number of
     * channels)
      * @param ac_nBufferSize the buffersize in samples
      * @param ac_bClipping if true, clips the output
      * @param ac_bDeleteWriter if true, deletes ac_pSink upon destruction or
     * replacing writer
      */
    AudioFormatWriterStream(AudioFormatWriter *const ac_pSink,
                            const unsigned ac_nBufferSize,
                            const bool ac_bDeleteWriter);

    /**
      * Destructor.
      * Deletes writer if specified to do so.
      */
    virtual ~AudioFormatWriterStream();

    /**
      * Implementation of the OutputStream method.
      */
    INLINE bool mf_bIsEndOfStream() const
    {
        return mv_bEOF;
    }
    /**
      * Implementation of the OutputStream method.
      */
    INLINE unsigned mf_nGetNumChannels() const
    {
        return m_Output.mf_nGetChannelCount();
    }

    /**
      * Implementation of the OutputStream method.
      */
    INLINE unsigned mf_nGetBufferSize() const
    {
        return m_Output.mf_nGetBufferSize();
    }

    /**
      * Implementation of the OutputStream method.
      */
    virtual void Write(const Stream &ac_Stream);

    /**
      * Replace the writer.
      * The new writer must have same bitmode and number of channels.
      * @param ac_pSink the new writer
      * @param ac_bDeleteWriter if true, deletes ac_pSink upon destruction or
     * replacing writer
      */
    void mp_ReplaceWriter(AudioFormatWriter *const ac_pSink,
                          const bool ac_bDeleteWriter);

    /**
      * Get the current writer.
      * @return an AudioFormatWriter pointer
      */
    INLINE const AudioFormatWriter *mf_pGetWriter() const
    {
        return m_pSource;
    }

    /**
      * @see AudioFormatConvertor
      */
    INLINE const tUnsignedBoolMap &mf_GetClippedChannels() const
    {
        return m_Output.mf_GetClippedChannels();
    }

    /**
      * @see AudioFormatConvertor
      */
    INLINE void mp_ResetClipped()
    {
        m_Output.mp_ResetClipped();
    }

    /**
      * @see AudioFormatConvertor
      */
    INLINE void mp_SetClipCallback(Callback *a_pCallback)
    {
        m_Output.mp_SetClipCallback(a_pCallback);
    }

private:
    AudioFormatWriter *m_pSource;
    AudioFormatConvertor m_Output;
    bool mv_bEOF;
    bool mv_bDeleteWriter;
    const appcore::CriticalSection mc_WriteLock;
};
}

#endif //#ifndef _APEX_SRC_LIB_STREAMAPP_AUDIOFORMATSTREAM_H__
