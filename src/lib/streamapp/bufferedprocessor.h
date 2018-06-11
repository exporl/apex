/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#ifndef _APEX_SRC_LIB_STREAMAPP_BUFFEREDPROCESSOR_H__
#define _APEX_SRC_LIB_STREAMAPP_BUFFEREDPROCESSOR_H__

#include "appcore/threads/criticalsection.h"
#include "stream.h"

namespace str3amapp
{
namespace containers
{
template <class tType, class tCritSection>
class CircularLogic;
}
}
using namespace str3amapp;

namespace streamapp
{

class Callback;
class BufferReader;
class BufferWriter;

/**
  * BufferedProcessing
  *   class providing an InputStream and OutputStream
  *   that read and write the same circular buffer.
  *   This is used to decouple two stream systems from
  *   each other, so that one can do heavy procesing in
  *   a seperate thread and write the result in here, while
  *   eg the soundcard thread just has to read from the buffer
  *   using a simple copy, wihout having to bother with the
  *   processing.
  *   @note see CircularLogic for multi-threading info

  *   TODO add callbacks for buffer overrun/underrun
  ************************************************************ */
class BufferedProcessing
{
public:
    /**
      * Constructor.
      * @param ac_nInputBufferSize the buffersize for the writer part
      * @param ac_nOutputBufferSize the buffersize reader part
      * @param ac_nChannels number of channels
      * @param ac_nBufferSize the buffer's max size
      */
    BufferedProcessing(const unsigned ac_nInputBufferSize,
                       const unsigned ac_nOutputBufferSize,
                       const unsigned ac_nChannels,
                       const unsigned ac_nBufferSize);

    /**
      * Destructor.
      */
    ~BufferedProcessing();

    /**
      * Get the InputStream part.
      * Used to read from the buffer.
      * @return a reference
      */
    InputStream &mf_GetInputStream();

    /**
      * Get the OutputStream part.
      * Used to write to the buffer.
      * @return a reference
      */
    OutputStream &mf_GetOutputStream();

    /**
      * Check how much samples are currently buffered.
      * @return number of samples available to the InputStream
      */
    unsigned mf_nGetNumInBuffer() const;

    /**
      * Check how much samples can currently be buffered.
      * @return number of samples available to the OutputStream
      */
    unsigned mf_nGetNumFree() const;

    /**
      * Empty the buffer.
      */
    void mp_Reset();

    /**
      * Get the size of the buffer.
      * @return size as set in ctor
      */
    INLINE const unsigned &mf_nGetBufferSize() const
    {
        return m_Buffer.mf_nGetBufferSize();
    }

    /**
      * Get the write size of the buffer.
      * @return size as set in ctor
      */
    INLINE const unsigned &mf_nGetInputBufferSize() const
    {
        return mc_nInputBufferSize;
    }

    /**
      * Get the read size of the buffer.
      * @return size as set in ctor
      */
    INLINE const unsigned &mf_nGetOutputBufferSize() const
    {
        return mc_nOutputBufferSize;
    }

    /**
      * Set the callback to call when the writer
      * tries to write more then available.
      * @param a_pCallback a pointer, 0 to reset
      */
    INLINE void mp_InstallBufferOverrunCallback(Callback *a_pCallback)
    {
        m_pOverRun = a_pCallback;
    }

    /**
      * Set the callback to call when the reader
      * tries to read more then available.
      * @param a_pCallback a pointer, 0 to reset
      */
    INLINE void mp_InstallBufferUnderrunCallback(Callback *a_pCallback)
    {
        m_pUnderRun = a_pCallback;
    }

private:
    friend class BufferReader;
    friend class BufferWriter;

    typedef containers::CircularLogic<double *, appcore::CriticalSection>
        mt_Fifo;

    const unsigned mc_nInputBufferSize;
    const unsigned mc_nOutputBufferSize;
    const unsigned mc_nChannels;
    const unsigned mc_nBufferSize;

    StreamBuf m_Buffer;
    mt_Fifo *m_pFifo;

    BufferReader *m_pReader;
    BufferWriter *m_pWriter;

    Callback *m_pOverRun;
    Callback *m_pUnderRun;

    BufferedProcessing(const BufferedProcessing &);
    BufferedProcessing &operator=(const BufferedProcessing &);
};
}

#endif //#ifndef _APEX_SRC_LIB_STREAMAPP_BUFFEREDPROCESSOR_H__
