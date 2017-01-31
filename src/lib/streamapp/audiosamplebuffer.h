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
 
#ifndef __AUDIOSAMPLEBUFFER_H__
#define __AUDIOSAMPLEBUFFER_H__

#include "typedefs.h"
#include "audioformat.h"
#include "streamappdefines.h"

namespace streamapp
{

  class AudioFormatReader;
  class AudioFormatWriter;

    /**
      * AudioSampleBuffer
      *   a Stream with basic operations for audio.
      *   Create an instance of this class using your samples
      *   if you want to do interesting stuff with it.
      *   Example:
      *   @code
      *   const Stream& str = blah.Read();
      *   AudioSampleBuffer calc( str );
      *   StreamType dRms, dMin, dMax;
      *   calc.mf_dCalculateRMS( 0, dRms );
      *   calc.mf_dCalculateMinMax( 0, dMin, dMax );
      *   //etc
      *   @endcode
      ******************************************************* */
  class AudioSampleBuffer : public Stream
  {
  public:
      /**
        * Constructor.
        * @param a_pSourceBuf the sample array(s) to operate on
        * @param ac_nChannels the number of arrays a_pSourceBuf points to
        * @param ac_nSamples the number of samples in the array(s) pointed to
        */
    AudioSampleBuffer( StreamType** a_pSourceBuf, const unsigned ac_nChannels, const unsigned ac_nSamples );

      /**
        * Constructor overload.
        * @param ac_Stream the MatrixAccess to operate on
        */
    AudioSampleBuffer( const Stream& ac_Stream );

      /**
        * Destructor.
        * Never deletes anything.
        */
    virtual ~AudioSampleBuffer();

      /**
        * Calculate RMS.
        * @param ac_nChannel the channel to calculate the RMS for
        * @param a_RMS the variable to receive the RMS
        */
    void mf_dCalculateRMS( const unsigned ac_nChannel, StreamType& a_RMS ) const;

      /**
        * Calculate minimum and maximum sample values.
        * @param ac_nChannel the channel to operate on
        * @param a_Min the variable to receive the min value
        * @param a_Max the variable to receive the max value
        */
    void mf_dCalculateMinMax( const unsigned ac_nChannel, StreamType& a_Min, StreamType& a_Max ) const;

      /**
        * Calculate the absolute maximum sample value.
        * @param ac_nChannel the channel to operate on
        * @param a_Abs the variable to receive the value
        */
    void mf_dCalculateAbsMinMax( const unsigned ac_nChannel, StreamType& a_Abs ) const;

      /**
        * Set all samples to the same value.
        * FIXME already exists in MatrixAccess
        @ note this changes the array elements.
        * @param ac_dValue the value to set the samples to.
        */
    void mp_SetAll( const StreamType ac_dValue );

      /**
        * Multiply each sample with the given value.
        * @note this changes the array elements.
        * @param ac_dGain the value to use (not in dB)
        */
    void mp_ApplyGain( const StreamType ac_dGain );

      /**
        * Multiply each sample with the given value.
        * @note this changes the array elements.
        * @param ac_nChannel the channel to operate on
        * @param ac_dGain the value to use (not in dB)
        */
    void mp_ApplyGain( const unsigned ac_nChannel, const StreamType ac_dGain );

      /**
        * Print contents to std::cout.
        */
    void mf_PrintToStdOut();

      /**
        * Write contents to a wavefile.
        * @param ac_sFile the filename
        * @param ac_lSampleRate the samplerate to set
        * @param ac_eBitMode the precision to use
        */
    void mf_WriteToWaveFile( const std::string& ac_sFile, const unsigned long ac_lSampleRate, const AudioFormat::mt_eBitMode ac_eBitMode );

      /**
        * Write contents to a raw binary file.
        * @param ac_sFile the filename
        */
    void mf_WriteToBinaryFile( const std::string& ac_sFile );

  private:
    AudioSampleBuffer( const AudioSampleBuffer& );
    AudioSampleBuffer& operator = ( const AudioSampleBuffer& );
 };

    /**
      * AudioSampleBufferStorage
      *   AudioSampleBuffer with memory of it's own.
      *   Use this if you need a seperate buffer for the AudioSamples
      *   eg if not using mf_DoProcessReplacing for an IStreamProcessor
      ***************************************************************** */
  class AudioSampleBufferStorage : public AudioSampleBuffer
  {
  public:
      /**
        * Constructor.
        * @param ac_nChannels the number of channels
        * @param ac_nSamples the number of samples
        */
    AudioSampleBufferStorage( const unsigned ac_nChannels, const unsigned ac_nSamples ) :
      AudioSampleBuffer( f_InitMem<StreamType>( ac_nChannels, ac_nSamples, true ), ac_nChannels, ac_nSamples )
    {
    }

      /**
        * Constructor overload.
        * @param ac_Rh the Stream to copy all samples from
        */
    AudioSampleBufferStorage( const Stream& ac_Rh ) :
      AudioSampleBuffer( f_InitMem<StreamType>( ac_Rh.mf_nGetChannelCount(), ac_Rh.mf_nGetBufferSize(), true ),
                          ac_Rh.mf_nGetChannelCount(), ac_Rh.mf_nGetBufferSize() )
    {
      mp_CopyFrom( ac_Rh );
    }

      /**
        * Destructor.
        */
    ~AudioSampleBufferStorage()
    {
      for( unsigned i = 0 ; i < AudioSampleBuffer::mf_nGetChannelCount() ; ++i )
        delete [] AudioSampleBuffer::mc_pSamplesArray[i];
      delete [] AudioSampleBuffer::mc_pSamplesArray;
    }

  private:
    AudioSampleBufferStorage( const AudioSampleBufferStorage& );
    AudioSampleBufferStorage& operator = ( const AudioSampleBufferStorage& );
  };

}

#endif //__AUDIOSAMPLEBUFFER_H__
