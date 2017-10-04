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

#ifndef __BASICBUFFER_H__
#define __BASICBUFFER_H__

#include "containers/matrix.h"
#include "stream.h"
#include "utils/checks.h"

namespace streamapp
{

/**
  * BasicBuffer
  *   processor for buffering Streams.
  *   Has functionality to set the buffer and can call a
  *   callback when the buffer is full.
  *   Because of exposing these two, it's easy to make eg
  *   a double buffering scheme with it.
  ******************************************************* */
class BasicBuffer : public IStreamProcessor
{
public:
    /**
      * Constructor.
      * @param ac_nChan the number of input channels
      * @param ac_nInputSize the number of input samples
      * @param ac_nBuffers the number of times to buffer the input to get a full
     * buffer (must be >= 1)
      */
    BasicBuffer(const unsigned ac_nChan, const unsigned ac_nInputSize,
                const unsigned ac_nBuffers);

    /**
      * Destructor.
      */
    virtual ~BasicBuffer();

    /**
      * Do the buffering.
      * Buffers until buffer is full, then calls back.
      * @param ac_StrToProc the stream to buffer
      * @return ac_StrToProc unchanged
      */
    virtual const Stream &mf_DoProcessing(const Stream &ac_StrToProc);

    /**
      * Set the buffer to use. Must have enough channels and samples.
      * No buffering if set to 0 (default).
      * @param ac_Buf the buffer or 0 for none.
      */
    void mp_SetBufferToFill(Stream *ac_Buf);

    /**
      * Set the processor to call when the buffer is full.
      * The processor's mf_DoProcessing is called with the buffer set in
     * mp_SetBufferToFill.
      * Nothing is done if processor is set to 0 (default)
      * @param ac_pCallbackProc the processor, or 0 for none.
      */
    void mp_SetCallback(IStreamProcessor *const ac_pCallbackProc);

    /**
      * Implementation of the IStreamProcessor method.
      */
    INLINE unsigned mf_nGetNumInputChannels() const
    {
        return mc_nChannels;
    }

    /**
      * Implementation of the IStreamProcessor method.
      */
    INLINE unsigned mf_nGetNumOutputChannels() const
    {
        return mc_nChannels;
    }

    /**
      * Implementation of the IStreamProcessor method.
      */
    INLINE unsigned mf_nGetBufferSize() const
    {
        return mc_nInputSize;
    }

    /**
      * Get the size in samples of the buffer.
      * @return the size
      */
    INLINE const unsigned &mf_nGetOutputSize() const
    {
        return mc_nOutputSize;
    }

    /**
      * Get the current number of streams in the buffer.
      * @return the number
      */
    INLINE const unsigned &mf_nGetCount() const
    {
        return m_nCount;
    }

    /**
      * Reset the buffer count.
      * This will re-start the buffering.
      */
    INLINE void mp_ResetCount()
    {
        m_nCount = 0;
    }

private:
    const unsigned mc_nBuffers;
    const unsigned mc_nInputSize;
    const unsigned mc_nOutputSize;
    const unsigned mc_nChannels;
    unsigned m_nCount;
    Stream *m_pResult;
    IStreamProcessor *m_pCallback;

    BasicBuffer(const BasicBuffer &);
    BasicBuffer &operator=(const BasicBuffer &);
};
}

#endif //#ifndef __BASICBUFFER_H__
