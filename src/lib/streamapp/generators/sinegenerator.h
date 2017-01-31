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
 
#ifndef __SINEGENERATOR_H__
#define __SINEGENERATOR_H__

#include "stream.h"
#include "typedefs.h"
#include "utils/math.h"

namespace streamapp
{

    /**
      * SineGenerator
      *  a basic sine generator implementation.
      *  Every Read() call returns ac_nBufferSize samples of a sine
      *  with the specified frequency and phase.
      ************************************************************* */
  class SineGenerator : public PositionableInputStream
  {
  public:
      /**
        * Constructor.
        * @param ac_nChan number of channels to generate
        * @param ac_nBufferSize the buffersize of the stream
        * @param ac_lSampleRate the samplerate used for the stream
        */
    SineGenerator( const unsigned       ac_nChan, 
                   const unsigned       ac_nBufferSize, 
                   const unsigned long  ac_lSampleRate );

      /**
        * Destructor.
        */
    ~SineGenerator();

      /**
        * Implementation of the InputStream method.
        */
    const Stream& Read();

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
    INLINE unsigned mf_nGetNumChannels() const
    {
      return mc_nChan;
    }

      /**
        * Implementation of the InputStream method.
        */
    unsigned mf_nGetBufferSize() const
    {
      return mc_nSize;
    }

      /**
        * Implementation of the InputStream method.
        */
    INLINE unsigned long mf_lGetNumSamplesRead() const
    {
      return 0;
    }

      /**
        * Implementation of the PositionableInputStream method.
        */
    INLINE unsigned long mf_lSampleRate() const
    {
      return sc_lInfinite;
    }

      /**
        * Implementation of the PositionableInputStream method.
        */
    INLINE unsigned long mf_lSamplesLeft() const
    {
      return sc_lInfinite;
    }

      /**
        * Implementation of the PositionableInputStream method.
        */
    INLINE unsigned long mf_lTotalSamples() const
    {
      return (unsigned long) mc_dSampleRate;
    }

      /**
        * Implementation of the PositionableInputStream method.
        */
    INLINE unsigned long mf_lCurrentPosition() const
    {
      return (unsigned long) m_dTime;
    }

      /**
        * Implementation of the PositionableInputStream method.
        */
    INLINE void mp_SeekPosition( const unsigned long ac_lPosition )
    {
      m_dTime = (double) ac_lPosition;
    }

      /**
        * Implementation of the PositionableInputStream method.
        * Looping doesn't make much sense so omit it for now
        */
    INLINE bool mf_bCanLoop() const
    {
      return false;
    }

      /**
        * Set the phase.
        * @param ac_dPhase the phase in radians
        */
    INLINE void mp_SetPhase( const double ac_dPhase )
    {
      m_dPhase = ac_dPhase;
    }

      /**
        * Set the frequnecy.
        * @param ac_dPhrequency the frequnecy in Hz
        */
    INLINE void mp_SetPhrequency( const double ac_dPhrequency )
    {
      m_dFrequency = ac_dPhrequency * math::gc_d2Pi;
    }

      /**
        * Set the amplitude.
        * @param ac_dAmplitude (linear scale)
        */
    INLINE void mp_SetAmplitude( const double ac_dAmplitude )
    {
      mv_dSignalAmp = ac_dAmplitude;
    }

  private:
    const unsigned  mc_nChan;
    const unsigned  mc_nSize;
    const double    mc_dSampleRate;
    double          mv_dSignalAmp;
    double          m_dFrequency;
    double          m_dPhase;
    double          m_dTime;
    StreamBuf       m_Buf;

    SineGenerator( const SineGenerator& );
    SineGenerator& operator = ( const SineGenerator& );
  };

}

#endif //#ifndef __SINEGENERATOR_H__
