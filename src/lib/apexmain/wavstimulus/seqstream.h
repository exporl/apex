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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_WAVSTIMULUS_SEQSTREAM_H_
#define _EXPORL_SRC_LIB_APEXMAIN_WAVSTIMULUS_SEQSTREAM_H_

#include "streamapp/audioformatstream.h"

#include "streamapp/containers/dynarray.h"

using namespace streamapp;

namespace apex
{
namespace stimulus
{
// note: could be optimized if all streams are connected to one single output
// then we could just use one object having all streams played after each other
// now there are as much streams as there are datablocks

typedef DynamicArray<unsigned long> tTriggers;

/**
  * ApexSeqStream
  *   InputStream capable of timed playing, behaves
  *   like a simple sequencer.
  *   Plays Parent after a given number of samples have elapsed.
  *   Plays zeroes before and after.
  *   Any number of triggers (=positions to start at) can be set,
  *   as long as they do not overlap.
  *************************************************************** */
class ApexSeqStream : public PositionableInputStream
{
public:
    /**
      * Constructor.
      * @param a_pParent the actual input to play.
      * @param if true, deletes a_pParent when deleted itself
      */
    ApexSeqStream(PosAudioFormatStream *a_pParent,
                  const bool ac_bDeleteParent = false);

    /**
      * Destructor.
      */
    ~ApexSeqStream();

    /**
      * Add a position to start playing the parent at.
      * Must be > previous position set + a_pParent->mf_lTotalSample.
      * @param ac_lStartAt the position in samples
      */
    void mp_SetStartAt(const unsigned long ac_lStartAt);

    /**
      * Implementation of the InputStream method.
      */
    const Stream &Read();

    /**
      * Implementation of the InputStream method.
      */
    bool mf_bIsEndOfStream() const
    {
        return mc_pParent->mf_bIsEndOfStream();
    }

    /**
      * Implementation of the InputStream method.
      */
    unsigned mf_nGetNumChannels() const
    {
        return mc_pParent->mf_nGetNumChannels();
    }

    /**
      * Implementation of the InputStream method.
      */
    unsigned mf_nGetBufferSize() const
    {
        return mc_pParent->mf_nGetBufferSize();
    }

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
    virtual unsigned long mf_lSampleRate() const
    {
        return mc_pParent->mf_lSampleRate();
    }

    /**
      * Implementation of the PositionableInputStream method.
      */
    bool mf_bCanLoop() const
    {
        return true;
    }

    /**
      * Implementation of the PositionableInputStream method.
      * @note this is forwarded to the parent: we don't want the whole
      * sequence to loop, just the parent!!
      */
    void mp_SetNumLoops(const unsigned ac_nLoops)
    {
        mc_pParent->mp_SetNumLoops(ac_nLoops);
    }

    /**
      * Implementation of the PositionableInputStream method.
      */
    unsigned mf_nGetNumLoops() const
    {
        return mc_pParent->mf_nGetNumLoops();
    }

    /**
      * Implementation of the PositionableInputStream method.
      */
    unsigned mf_nGetNumLooped() const
    {
        return mc_pParent->mf_nGetNumLooped();
    }

    /**
      * Implementation of the PositionableInputStream method.
      */
    bool mf_bIsLooping() const
    {
        return mc_pParent->mf_bIsLooping();
    }

    /**
      * Replace the parent reader.
      * @see PositionableInputStream::mp_ReplaceReader()
      */
    void mp_ReplaceReader(PositionableAudioFormatReader *const ac_pSource,
                          const bool ac_bDeleteReader)
    {
        mc_pParent->mp_ReplaceReader(ac_pSource, ac_bDeleteReader);
    }

    /**
      * Get the parent reader.
      * @see PositionableInputStream::mf_pGetReader()
      */
    const AudioFormatReader *mf_pGetReader() const
    {
        return mc_pParent->mf_pGetReader();
    }

private:
    PosAudioFormatStream *const mc_pParent;
    const bool mc_bDeleteParent;
    tTriggers m_Triggers;
    unsigned mv_nStartIndex;
    unsigned long mv_lTotalSamples;
    unsigned long mv_lStartAt;
    unsigned long mv_lCurrentPosition;
    bool mv_bParentIsPlaying;
    bool mv_bParentNeedsReset;
};
}
}

#endif //#ifndef _EXPORL_SRC_LIB_APEXMAIN_WAVSTIMULUS_SEQSTREAM_H_
