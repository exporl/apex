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
 
#include "spectrum.h"
#include "utils/math.h"

using namespace streamapp;

FFT::FFT( const unsigned ac_nChannels, const unsigned ac_nBufferSize ) :
    StreamProcessor( ac_nChannels, ac_nBufferSize ),
  m_FFTObj( ac_nBufferSize ),
  m_ReAndIm( ac_nChannels, ac_nBufferSize )
{
  assert( ac_nBufferSize % 2 == 0 );
}

FFT::~FFT()
{

}

const Stream& FFT::mf_DoProcessing( const Stream& ac_StrToProc )
{
  assert( ac_StrToProc.mf_nGetChannelCount() >= mf_nGetNumInputChannels() );
  assert( ac_StrToProc.mf_nGetBufferSize() == mf_nGetBufferSize() );

    //calc discrete FTC
  for( unsigned j = 0 ; j < mf_nGetNumInputChannels() ; ++j )
    m_FFTObj.do_fft( m_ReAndIm.mf_pGetArray()[ j ], ac_StrToProc.mf_pGetArray()[ j ] );

  return m_ReAndIm;
}

IFFT::IFFT( const unsigned ac_nChannels, const unsigned ac_nBufferSize ) :
    StreamProcessor( ac_nChannels, ac_nBufferSize ),
  m_FFTObj( ac_nBufferSize ),
  m_Result( ac_nChannels, ac_nBufferSize )
{

}

IFFT::~IFFT()
{

}

const Stream& IFFT::mf_DoProcessing( const Stream& ac_StrToProc )
{
  assert( ac_StrToProc.mf_nGetChannelCount() >= mf_nGetNumInputChannels() );
  assert( ac_StrToProc.mf_nGetBufferSize() == mf_nGetBufferSize() );

    //calc discrete inverse fft
  for( unsigned j = 0 ; j < mf_nGetNumInputChannels() ; ++j )
    m_FFTObj.do_ifft( ac_StrToProc.mf_pGetArray()[ j ], m_Result.mf_pGetArray()[ j ] );

  return m_Result;
}

Spectra::Spectra( const unsigned ac_nChan, const unsigned ac_nInputSize, const bool ac_bPowerinDb ) :
    StreamProcessor( ac_nChan, ac_nInputSize ),
  m_Power( ac_nChan, ac_nInputSize / 2 ),
  m_Peak( ac_nChan, ac_nInputSize / 2 ),
  m_Phase( ac_nChan, ac_nInputSize / 2 ),
  mc_bDb( ac_bPowerinDb ),
  mc_dFFTSize( (double) ac_nInputSize ),
  mc_dFFTSizePow2( (double) ac_nInputSize * (double) ac_nInputSize ),
  mc_nSpectrumSize( ac_nInputSize / 2 )
{
  assert( ac_nInputSize % 2 == 0 ); //a bit late (everything is already initialized) but must be valid
}

Spectra::~Spectra()
{

}

const Stream& Spectra::mf_DoProcessing( const Stream& ac_StrToProc )
{
  assert( ac_StrToProc.mf_nGetChannelCount() >= mf_nGetNumInputChannels() );
  assert( ac_StrToProc.mf_nGetBufferSize() == mf_nGetBufferSize() );

  for( unsigned j = 0 ; j < mf_nGetNumInputChannels() ; ++j )
  {
    for( unsigned i = 0 ; i < mc_nSpectrumSize ; ++i )
    {
        //get Real and Imaginary parts
      const double dRe = ac_StrToProc( j, i );
      double dIm = -ac_StrToProc( j, mc_nSpectrumSize + i );
      if( i == 0 )
        dIm = 0.0;

        //get spectra
      const double dAbsPart = dRe*dRe + dIm*dIm;

      double dPeakSpectrum = 2.0 * sqrt( dAbsPart )  / mc_dFFTSize;
      if( mc_bDb )
        dPeakSpectrum = 20.0 * log10( dPeakSpectrum ); //dB..

      double dPowerSpectrum = 2.0 * dAbsPart / mc_dFFTSizePow2;

      if( i == 0 )
      {
        dPeakSpectrum  = dPeakSpectrum / 2.0;
        dPowerSpectrum = dPowerSpectrum / 2.0;
      }

        //get phase
      double dPhase = math::f_dDRadiansToDegrees( atan2( dIm, dRe ) );

        //alle negatieve hoeken omzetten naar positieve
      if(dIm < 0 )
        dPhase += 360.0;

        //set result
      m_Peak.mp_Set( j, i, dPeakSpectrum );
      m_Power.mp_Set( j, i, dPowerSpectrum );
      m_Phase.mp_Set( j, i, dPhase );
    }
  }

  return ac_StrToProc;
}

Spectrum::Spectrum( const unsigned ac_nChan, const unsigned ac_nInputSize, const bool ac_bPowerinDb ) :
  Spectra( ac_nChan, ac_nInputSize, ac_bPowerinDb ),
  m_FFTObj( ac_nChan, ac_nInputSize )
{

}

Spectrum::~Spectrum()
{
}

const Stream& Spectrum::mf_DoProcessing( const Stream& ac_StrToProc )
{
  Spectra::mf_DoProcessing( m_FFTObj.mf_DoProcessing( ac_StrToProc ) );
  return ac_StrToProc;
}
