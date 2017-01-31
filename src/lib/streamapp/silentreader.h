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
 
#ifndef __SILENTREADER_H__
#define __SILENTREADER_H__

#include "defines.h"
#include "audioformat.h"

namespace streamapp
{

    /**
      * SilentReader
      *   a reader that only produces zeros.
      *   TODO optimize by using MSBfloat64
      ************************************** */
  class SilentReader : public PositionableAudioFormatReader
  {
  public:
      /**
        * Constructor.
        * @param ac_nChan the number of channels to generate
        * @param ac_nFs the samplerate to work at
        * @param ac_dLengthInMilliSeconds the length
        */
    SilentReader( const unsigned ac_nChan, const unsigned ac_nFs, const double ac_dLengthInMilliSeconds );

      /**
        * Constructor.
        * @param ac_nChan the number of channels to generate
        * @param ac_nFs the samplerate to work at
        * @param ac_nToGenerate the number of samples to generate
        */
    SilentReader( const unsigned ac_nChan, const unsigned ac_nFs, const unsigned long ac_nToGenerate );

      /**
        * Destructor.
        */
    virtual ~SilentReader();

      /**
        * Implementation of the AudioFormat method.
        */
    INLINE virtual unsigned mf_nChannels() const
    {
      return mc_nChannels;
    }

      /**
        * Implementation of the AudioFormat method.
        */
    INLINE unsigned long mf_lSampleRate() const
    {
      return mc_lSampleRate;
    }

      /**
        * Implementation of the AudioFormat method.
        */
    INLINE AudioFormat::mt_eBitMode mf_eBitMode() const
    {
      return AudioFormat::MSBint32;
    }

      /**
        * Implementation of the AudioFormatReader method.
        */
    unsigned long Read( void** a_pBuf, const unsigned ac_nSamples );

      /**
        * Implementation of the PositionableAudioFormatReader method.
        */
    INLINE unsigned long mf_lSamplesLeft() const;

      /**
        * Implementation of the PositionableAudioFormatReader method.
        */
    INLINE unsigned long mf_lTotalSamples() const;

      /**
        * Implementation of the PositionableAudioFormatReader method.
        */
    INLINE unsigned long mf_lCurrentPosition() const;

      /**
        * Implementation of the PositionableAudioFormatReader method.
        */
    void  mp_SeekPosition( const unsigned long ac_nPosition );

  private:
    const unsigned long mc_lSampleRate;
    const unsigned long mc_lSamples;
    const unsigned      mc_nChannels;
    unsigned long       mv_lGenerated;

    SilentReader( const SilentReader& );
    SilentReader& operator = ( const SilentReader& );
  };

}

#endif //#ifndef __SILENTREADER_H__
