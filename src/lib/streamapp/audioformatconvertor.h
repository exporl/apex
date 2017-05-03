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

#ifndef _APEX_SRC_LIB_STREAMAPP_AUDIOFORMATCONVERTOR_H__
#define _APEX_SRC_LIB_STREAMAPP_AUDIOFORMATCONVERTOR_H__

#include "typedefs.h"
#include "audioformat.h"
#include "streamappdefines.h"

namespace streamapp
{

  class Callback;

    /**
      * AudioFormatConvertor
      *   converts Stream from/to AudioFormatReader/Writer.
      *   Supports normalizing (-1.0 - 1.0) and checking for clipped samples.
      *   @see AudioFormatReaderStream.
      *********************************************************************** */
  class AudioFormatConvertor : public Stream
  {
  public:
      /**
        * Constructor.
        * @param ac_nChannels the number of channels to use
        * @param ac_nSamples the buffersize to use
        */
    AudioFormatConvertor( const unsigned ac_nChannels, const unsigned ac_nSamples );

      /**
        * Destructor.
        */
    ~AudioFormatConvertor();

      /**
        * Read from a source.
        * After a read, the class' array(s) contains new samples which can be accessed
        * using all normal MatrixAccess methods.
        * If the source is finished, the method returns less samples then ac_nSamples.
        * @param ac_pSource the AudioFormatReader to read samples from
        * @param ac_nSamples the number of samples to read, must be <= buffersize
        * @param ac_nSourceOffset the offset to start reading samples , must be <= buffersize
        * @return the number of samples actually read
        */
    unsigned ReadFromSource(  AudioFormatReader* const ac_pSource, const unsigned ac_nSamples,
                                    const unsigned ac_nSourceOffset);

       /**
        * Write to a source.
        * Before writing, place new samples into the class' array(s) using MatrixAccess' methods,
        * then call this method to write the samples to the writer.
        * @param a_cpSink the AudioFormatWriter to write samples to
        * @param ac_nSamples the number of samples to write, must be <= buffersize
        * @return the number of samples actually written
        */
    unsigned WriteToSource(   AudioFormatWriter* const a_cpSink, const unsigned ac_nSamples);

      /**
        * Get the clipped channels.
        * Has only effect when WriteToSource is called with ac_bClipping = true.
        * @return a map containing clipped channels
        */
    const tUnsignedBoolMap& mf_GetClippedChannels() const;

      /**
        * Set all clipping on channels to false.
        */
    void mp_ResetClipped();

      /**
        * Set callback to call when clipping occurs.
        * @warning if this is for the soundcard stream, make sure not to block too long,
        * or buffer underruns will occur!
        * @warning callback will be called *everytime* a buffer clips
        * @param a_pCallback the callback or 0 for none/reset
        */
    void mp_SetClipCallback( Callback* a_pCallback );

      /**
        * Set the gain to apply to the samples that are read.
        * This doesn't slow slow down the conversion for most types
        * ( it does for for 64bit and conversions that don't normalize ),
        * but is' always way faster then applying gain to the Stream afterwards
        * using a processor since that introduces an extra loop.
        * Setting the gain is not thread-safe here,
        * use AudioFormatStream to set it, there it is protected.
        * @param ac_dGain the gain, linear scale
        */
    INLINE void mp_SetReadGain( const double ac_dGain )
    {
      mv_dGain = ac_dGain;
    }

      /**
        * Get the current gain.
        * @see mp_SetReadGain()
        * @return the gain
        */
    INLINE const double& mf_dGetReadGain() const
    {
      return mv_dGain;
    }

  protected:
    MatrixStorage<int>    m_WorkHere;
    tUnsignedBoolMap      m_Clipped;
    unsigned long         mv_lWrittenSoFar;
    bool                  mv_bCalledBack;
    Callback*             mv_pCallback;
    double                mv_dGain;

    AudioFormatConvertor( const AudioFormatConvertor& );
    AudioFormatConvertor& operator= ( const AudioFormatConvertor& );
  };

}

#endif //#ifndef _APEX_SRC_LIB_STREAMAPP_AUDIOFORMATCONVERTOR_H__
