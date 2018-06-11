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

#ifndef __STREAMAPPBASE_H_
#define __STREAMAPPBASE_H_

#include "../audioformatstream.h"
#include "../eofcheck.h"
#include "../stream.h"

#include "callback.h"

/*                   *                          *\

  WARNING none of these classes are threadsafe
  (where threadsafe means protected against simultaneous reading and writing)

\*                   *                          */

namespace streamapp
{

/**
  * Basic callback
  *   1 input, 1 output and 1 processor.
  *   The last two can be omitted for testing,
  *   eg if no processor is specified, it just
  *   does input->output.
  *   Optionally deletes the items put in.
  ******************************************** */
class Channel : public Callback
{
public:
    /**
      * Constructor.
      * @param ac_bDeleteContent if true, deletes all items when killed or when
     * removed
      */
    Channel(const bool ac_bDeleteContent = false);

    /**
      * Destructor.
      * Might delete items it told to do so.
      */
    virtual ~Channel();

    /**
      * Set the sink.
      * @param ac_pSink an OutputStream or 0 for none
      */
    void mp_SetSink(OutputStream *const ac_pSink);

    /**
      * Set the source.
      * @param ac_pSource an InputStream or 0 for none
      */
    void mp_SetSource(InputStream *const ac_pSource);

    /**
      * Set the processor.
      * @param ac_pProc an IStreamProcessor or zero for none
      */
    void mp_SetProcessor(IStreamProcessor *const ac_pProc);

    /**
      * Get the current sink.
      * @return pointer
      */
    INLINE OutputStream *mf_pGetSink() const
    {
        return m_pPlaySink;
    }

    /**
      * Get the current source.
      * @return pointer
      */
    INLINE InputStream *mf_pGetSource() const
    {
        return m_pPlaySource;
    }

    /**
      * Get the current processor.
      * @return pointer
      */
    INLINE IStreamProcessor *mf_pGetProcessor() const
    {
        return m_pProcessor;
    }

    /**
      * Implementation of the Callback method.
      */
    void mf_Callback();

private:
    const bool mc_bDeleteContent;
    InputStream *m_pPlaySource;
    OutputStream *m_pPlaySink;
    IStreamProcessor *m_pProcessor;

    Channel(const Channel &);
    Channel &operator=(const Channel &);
};

/**
  * StreamAppChannel
  *   a Callback like in StreamApp v1.
  *   multiple input streams are read parallel, bundeled into one stream and
  *serve
  *   as input for any number of sequential processors. The output of last
  *processor,
  *   or the input stream if no processors present, is distributed over the
  *outputstreams
  *   Everything is used in the order it's added.
  ***************************************************************************************
  */
class StreamAppChannel : public Callback
{
public:
    /**
      * Constructor.
      */
    StreamAppChannel(const bool ac_bDeleteContent = true,
                     const bool ac_bSerialOutput = false);

    /**
      * Destructor.
      */
    virtual ~StreamAppChannel();

    /**
      * Add a processor after the previous one.
      * @param ac_pProc the processor to add
      */
    void mp_AddProcessor(IStreamProcessor *const ac_pProc);

    /**
      * Add an InputStream next to the previous one.
      * @param ac_pSource the InputStream to add
      */
    void mp_AddInputStream(InputStream *const ac_pSource);

    /**
      * Add an OutputStream next to the previous one.
      * @param ac_pSink the OutputStream to add
      */
    void mp_AddOutputStream(OutputStream *const ac_pSink);

    /**
      * Remove an input at the specified index.
      * @param ac_nIndex the index
      */
    void mp_RemoveInput(const unsigned ac_nIndex);

    /**
      * Remove an output at the specified index.
      * @param ac_nIndex the index
      */
    void mp_RemoveOutput(const unsigned ac_nIndex);

    /**
      * Remove a processor at the specified index.
      * @param ac_nIndex the index
      */
    void mp_RemoveProcessor(const unsigned ac_nIndex);

    /**
      * Get the number of inputs.
      * @return the number
      */
    INLINE const unsigned &mf_nGetNumInputs() const;

    /**
      * Get the number of outputs.
      * @return the number
      */
    INLINE const unsigned &mf_nGetNumOutputs() const;

    /**
      * Get the number of processors.
      * @return the number
      */
    INLINE const unsigned &mf_nGetNumProcessors() const;

    /**
      * Check if the configuration is valid.
      * Better assert this true before running Callback or nasty things will
     * happen.
      * Basically checks if I->P->O has the same number of channels everywhere.
      * @return true if ok
      */
    bool mf_bCheckValidConfig() const;

    /**
      * Implementation of the Callback method.
      */
    INLINE void mf_Callback()
    {
        m_Channel.mf_Callback();
    }

private:
    struct mt_Detail;
    struct mt_OwnedDetail;
    struct mt_NotOwnedDetail;
    mt_Detail *m_pDetail;
    Channel m_Channel;

    StreamAppChannel(const StreamAppChannel &);
    StreamAppChannel &operator=(const StreamAppChannel &);
};
}

#endif //#ifndef __STREAMAPPBASE_H_
