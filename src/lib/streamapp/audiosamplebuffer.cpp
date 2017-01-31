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

#include "file/binaryfile.h"

#include "utils/stringutils.h"

#include "audiosamplebuffer.h"
#include "stream.h"
#include "streamappfactory.h"

#include <math.h>

#include <QtGlobal>

#include <iostream>

using namespace streamapp;

AudioSampleBuffer::AudioSampleBuffer( StreamType** ac_pSourceBuf, const unsigned ac_nChannels, const unsigned ac_nSamples ) :
  Stream( ac_pSourceBuf, ac_nChannels, ac_nSamples )
{
}

AudioSampleBuffer::AudioSampleBuffer( const Stream& a ) :
  Stream( a )
{
}

AudioSampleBuffer::~AudioSampleBuffer()
{
}

void AudioSampleBuffer::mf_dCalculateRMS( const unsigned ac_nChannel, StreamType& a_RMS ) const
{
  Q_ASSERT( ac_nChannel < mf_nGetChannelCount() );
  const unsigned c_nSamples = mf_nGetBufferSize();
  StreamType* src = mf_pGetArray()[ ac_nChannel ];

  double dSum = 0.0;
  for( unsigned j = 0 ; j < c_nSamples ; ++j )
  {
    const StreamType dSample = *src++;
    dSum += dSample * dSample;
  }
  a_RMS = sqrt( dSum / (double) c_nSamples );
}

void AudioSampleBuffer::mf_dCalculateMinMax( const unsigned ac_nChannel, double& a_Min, double& a_Max ) const
{
  Q_ASSERT( ac_nChannel < mf_nGetChannelCount() );
  const unsigned c_nSamples = mf_nGetBufferSize();
  StreamType* src = mf_pGetArray()[ ac_nChannel ];

  StreamType dMin = src[ 0 ];
  StreamType dMax = dMin;
  for( unsigned j = 1 ; j < c_nSamples ; ++j )
  {
    const StreamType dSample = src[ j ];
    if( dSample > dMax )
      dMax = dSample;
    if( dSample < dMin )
      dMin = dSample;
  }
  a_Min = dMin;
  a_Max = dMax;
}

void AudioSampleBuffer::mf_dCalculateAbsMinMax( const unsigned ac_nChannel, double& a_Abs ) const
{
  Q_ASSERT( ac_nChannel < mf_nGetChannelCount() );
  const unsigned c_nSamples = mf_nGetBufferSize();
  StreamType* src = mf_pGetArray()[ ac_nChannel ];

  StreamType dAbs = fabs( src[ 0 ] );
  for( unsigned j = 1 ; j < c_nSamples ; ++j )
  {
    const StreamType dSample = fabs( src[ j ] );
    if( dSample > dAbs )
      dAbs = dSample;
  }
  a_Abs = dAbs;
}

void AudioSampleBuffer::mp_SetAll( const double ac_dValue )
{
  const unsigned c_nChannels = mf_nGetChannelCount();
  const unsigned c_nSamples  = mf_nGetBufferSize();

  for( unsigned j = 0 ; j < c_nSamples ; ++j )
    for( unsigned i = 0 ; i < c_nChannels ; ++i )
      Stream::mp_Set( i, j, ac_dValue );
}

void AudioSampleBuffer::mp_ApplyGain( const double ac_dGain )
{
  for( unsigned j = 0 ; j < mf_nGetChannelCount() ; ++j )
    mp_ApplyGain( j, ac_dGain );
}

void AudioSampleBuffer::mp_ApplyGain( const unsigned ac_nChannel, const double ac_dGain )
{
  Q_ASSERT( ac_nChannel < mf_nGetChannelCount() );

  if( ac_dGain == 1.0 )
    return;

  const unsigned c_nSamples = mf_nGetBufferSize();
  StreamType* src = mf_pGetArray()[ ac_nChannel ];

  if( ac_dGain != 0.0 )
  {
    for( unsigned j = 0 ; j < c_nSamples ; ++j )
      src[ j ] *= ac_dGain;
  }
  else
    Stream::mp_Clear( ac_nChannel );
}

void AudioSampleBuffer::mf_PrintToStdOut()
{
  const unsigned c_nChannels = mf_nGetChannelCount();
  const unsigned c_nSamples  = mf_nGetBufferSize();

  for( unsigned j = 0 ; j < c_nSamples ; ++j )
  {
    std::cout << "sample " << toString( j ) << " :\t";
    for( unsigned i = 0 ; i < c_nChannels ; ++i )
    {
      std::cout << toString( mf_Get( i, j ) ) << "\t";
    }
    std::cout << std::endl;
  }
}

void AudioSampleBuffer::mf_WriteToWaveFile( const std::string& ac_sFile, const unsigned long ac_lSampleRate, const AudioFormat::mt_eBitMode ac_eBitMode )
{
  const unsigned c_nChannels = mf_nGetChannelCount();
  const unsigned c_nSamples  = mf_nGetBufferSize();

  OutputStream* p = StreamAppFactory::sf_pInstance()->mf_pWriterStream( ac_sFile, c_nChannels, ac_lSampleRate, c_nSamples, ac_eBitMode );

  p->Write( *this );

  delete p;
}

void AudioSampleBuffer::mf_WriteToBinaryFile( const std::string& ac_sFile )
{
  BinaryOutputFile file;
  if( file.mp_bOpen( ac_sFile, mf_nGetChannelCount() ) )
  {
    file.Write( *this );
    file.mp_bClose();
  }
}
