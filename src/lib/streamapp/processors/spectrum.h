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
 
#ifndef SPECTRUMIZER_H
#define SPECTRUMIZER_H

#include "stream.h"
#include "audiosamplebuffer.h"
#include "processors/FFTReal/FFTReal2.h"
#include "processors/processor.h"
#include "containers/matrix.h"
#include "multiproc.h"
#include <math.h>

namespace streamapp
{

    /**
      * FFT
      *   calculates fft (real and imaginary coefficients).
      *   @see FFTReal.h
      ***************************************************** */
  class FFT : public StreamProcessor
  {
  public:
      /**
        * Constructor.
        * @param ac_nChannels the number of channels
        * @param ac_nBufferSize the number of samples
        */
    FFT( const unsigned ac_nChannels, const unsigned ac_nBufferSize );

      /**
        * Destructor.
        */
    ~FFT();

      /**
        * Implementation of the IStreamProcessor method.
        * @note this returns frequency domain samples so don't send it to a soundcard or so
        */
    const Stream& mf_DoProcessing( const Stream& ac_StrToProc );

      /**
        * Get the result of the last mf_DoProcessing() call.
        * @return the spectrum
        */
    INLINE const Stream& mf_GetReAndIm() const
    {
      return m_ReAndIm;
    }

  private:
    FFTReal<StreamType> m_FFTObj;
    StreamBuf           m_ReAndIm;
  };

    /**
      * IFFT
      *   inverse of FFT.
      *   @note IFFT( FFT( x ) ) = x * length (x).
      ******************************************** */
  class IFFT : public StreamProcessor
  {
  public:
      /**
        * Constructor.
        * @param ac_nChannels the number of channels
        * @param ac_nBufferSize the number of samples
        */
    IFFT( const unsigned ac_nChannels, const unsigned ac_nBufferSize );

      /**
        * Destructor.
        */
    ~IFFT();

      /**
        * Implementation of the IStreamProcessor method.
        * @note this expects frequency domain samples as input
        */
    const Stream& mf_DoProcessing( const Stream& ac_StrToProc );

  private:
    FFTReal<StreamType> m_FFTObj;
    StreamBuf           m_Result;
  };

    /**
      * PowerAndPhase
      *   calculates power spectrum, peak amplitude spectrum and phase.
      *   Needs real and imaginary input (eg FFT output).
      ***************************************************************** */
  class Spectra : public StreamProcessor
  {
  public:
      /**
        * Constructor.
        * @param ac_nChannels the number of channels
        * @param ac_nBufferSize the number of samples
        * @param ac_bPowerinDb if true, returns the powerspectrum with dB values
        */
    Spectra( const unsigned ac_nChannels, const unsigned ac_nBufferSize, const bool ac_bPowerinDb = false );

      /**
        * Destructor.
        */
    virtual ~Spectra();

      /**
        * Implementation of the IStreamProcessor method.
        * @note this returns ac_StrToProc unchanged
        */
    virtual const Stream& mf_DoProcessing( const Stream& ac_StrToProc );

      /**
        * Get the peak amplitude spectrum.
        * Valid after calling mf_DoProcessing().
        * @return the spectrum.
        */
    INLINE const Stream& mf_GetPeakAmplitudeSpectrum() const
    {
      return m_Peak;
    }

      /**
        * Get the power spectrum.
        * Valid after calling mf_DoProcessing().
        * @return the spectrum.
        */
    INLINE const Stream& mf_GetPowerSpectrum() const
    {
      return m_Power;
    }

      /**
        * Get the phase amplitude spectrum.
        * Valid after calling mf_DoProcessing().
        * @return the spectrum.
        */
    INLINE const Stream& mf_GetPhase() const
    {
      return m_Phase;
    }

  protected:
    StreamBuf       m_Power;
    StreamBuf       m_Peak;
    StreamBuf       m_Phase;
    const bool      mc_bDb;
    const double    mc_dFFTSize;
    const double    mc_dFFTSizePow2;
    const unsigned  mc_nSpectrumSize;  //FFTSize / 2

    Spectra( const Spectra& );
    Spectra& operator = ( const Spectra& );
  };

    /**
      * Spectrum
      *   combinates FFT and Spectra.
      ******************************* */
  class Spectrum : public Spectra
  {
  public:
      /**
        * Constructor.
        * @param ac_nChannels the number of channels
        * @param ac_nBufferSize the number of samples
        * @param ac_bPowerinDb if true, returns the powerspectrum with dB values
        */
    Spectrum( const unsigned ac_nChannels, const unsigned ac_nBufferSize, const bool ac_bPowerinDb = false );

      /**
        * Destructor.
        */
    ~Spectrum();

      /**
        * Implementation of the IStreamProcessor method.
        * @note this returns ac_StrToProc unchanged
        */
    const Stream& mf_DoProcessing( const Stream& ac_StrToProc );

      /**
        * Get the spectrum.
        * Returns real and imaginary part.
        * Valid after calling mf_DoProcessing().
        * @return the spectrum.
        */
    INLINE const Stream& mf_GetReAndIm() const
    {
      return m_FFTObj.mf_GetReAndIm();
    }

  private:
    FFT m_FFTObj;

    Spectrum( const Spectrum& );
    Spectrum& operator = ( const Spectrum& );
  };

    /**
      * FrequencyDomainProcessor
      *   does FFT, processing, IFFT.
      *   All processors added with SlotProcessor methods
      *   will receive real and imaginary parts as input.
      *************************************************** */
  class FrequencyDomainProcessor : public SlotProcessor
  {
  public:
      /**
        * Constructor.
        * @param ac_nChannels the number of channels
        * @param ac_nBufferSize the number of samples
        * @param ac_nMaxProcessors passed to SlotProcessor
        */
    FrequencyDomainProcessor( const unsigned ac_nChannels, const unsigned ac_nBufferSize, const unsigned ac_nMaxProcessors ) :
        SlotProcessor( ac_nMaxProcessors ),
      m_Fft( ac_nChannels, ac_nBufferSize ),
      m_IFft( ac_nChannels, ac_nBufferSize )
    {
    }

      /**
        * Destructor.
        */
    virtual ~FrequencyDomainProcessor()
    {
    }

      /**
        * Implementation of the IStreamProcessor method.
        */
    virtual const Stream& mf_DoProcessing( const Stream& ac_StrToProc )
    {
      return m_IFft.mf_DoProcessing( SlotProcessor::mf_DoProcessing( m_Fft.mf_DoProcessing( ac_StrToProc ) ) );
    }

  protected:
    FFT  m_Fft;
    IFFT m_IFft;

  private:
    FrequencyDomainProcessor( const FrequencyDomainProcessor& );
    FrequencyDomainProcessor& operator = ( const FrequencyDomainProcessor& );
  };

    /**
      * BandStopFilter
      *   just for testing, doesn't contain windowing
      *   and is mathematically incorrect.
      *   Use on coefficents of a Multiplier to do filtering
      *   TODO finish this
      ****************************************************** */
  class BandStopFilter
  {
  public:
    BandStopFilter( Stream& a_Coefficients, const double ac_dNyquist ) :
      m_Coefficients( a_Coefficients ),
      mc_dNyquist( ac_dNyquist ),
      mc_FreqMiddle( a_Coefficients.mf_nGetBufferSize() / 2 ),
      mv_nStart( 0 ),
      mv_nStop( 0 ),
      mv_nFreq( 0 ),
      mv_nQFac( 0 )
    {
      AudioSampleBuffer buf( a_Coefficients );
      buf.mp_SetAll( 1.0 );
    }

    void mp_SetGain( const double ac_dGain )
    {
      mv_dGain = ac_dGain;
      mp_Update();
    }

    void mp_SetFrequency( const double ac_dFrequency )
    {
      mv_nFreq = (unsigned) ( ( ac_dFrequency / mc_dNyquist ) * mc_FreqMiddle );
      mp_Update();
    }

    void mp_SetQFactor( const double ac_dQFactor )
    {
      mv_nQFac = (unsigned) ( mc_dNyquist * ac_dQFactor ) / 2;
      mp_Update();
    }

    double mf_dGetGain() const
    {
      return mv_dGain;
    }

    double mf_dGetFrequency() const
    {
      return ( mv_nFreq / mc_FreqMiddle ) * mc_dNyquist;
    }

    double mf_dGetQFactor() const
    {
      return mv_nQFac * 2 / mc_dNyquist;
    }

    void mp_Update()
    {
        //reset previous range
      for( unsigned j = 0 ; j < m_Coefficients.mf_nGetChannelCount() ; ++j )
        for( unsigned i = mv_nStart ; i < mv_nStop ; ++ i )
          m_Coefficients.mp_Set( j, i, 1.0 );

        //calculate new
      if( mv_nQFac > mv_nFreq )
        mv_nStart = 0;
      else
        mv_nStart = (unsigned) ( mv_nFreq - mv_nQFac / 2 );
      if( mv_nFreq + mv_nQFac < m_Coefficients.mf_nGetBufferSize() )
        mv_nStop  = (unsigned) ( mv_nFreq + mv_nQFac / 2 );
      else
        mv_nStop = m_Coefficients.mf_nGetBufferSize();

        //set gain
      for( unsigned j = 0 ; j < m_Coefficients.mf_nGetChannelCount() ; ++j )
        for( unsigned i = mv_nStart ; i < mv_nStop ; ++ i )
          m_Coefficients.mp_Set( j, i, mv_dGain );
    }

  private:
    Stream& m_Coefficients;
    const double mc_dNyquist;
    const double mc_FreqMiddle;
    unsigned mv_nStart;
    unsigned mv_nStop;
    unsigned mv_nFreq;
    unsigned mv_nQFac;
    double   mv_dGain;

    BandStopFilter( const BandStopFilter& );
    BandStopFilter& operator = ( const BandStopFilter& );
  };


}

#endif //#ifndef SPECTRUMIZER_H
