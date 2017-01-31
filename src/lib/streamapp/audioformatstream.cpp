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

#include "appcore/threads/locks.h"

#include "utils/checks.h"

#include "audioformat.h"
#include "audioformatstream.h"
#include "audiosamplebuffer.h"

#include <math.h>

#include <QtGlobal>

using namespace utils;
using namespace appcore;
using namespace streamapp;

AudioFormatReaderStream::AudioFormatReaderStream( AudioFormatReader* const ac_cpSource, const unsigned ac_nBufferSize,
                                                  const bool ac_bNormalize, const bool ac_bDeleteReader ) :
    InputStream(),
  m_pSource( PtrCheck( ac_cpSource ) ),
  m_Output( ac_cpSource->mf_nChannels(), ac_nBufferSize ),
  mc_bNormalize( ac_bNormalize ),
  mv_nReadLastTime( 0 ),
  mv_bDeleteReader( ac_bDeleteReader ),
  mv_bEOF( false ),
  mv_bOutputCleared( false ),
  mc_ReadLock()
{
}

AudioFormatReaderStream::~AudioFormatReaderStream()
{
  const Lock L( mc_ReadLock );
  if( mv_bDeleteReader )
    delete m_pSource;
}

void AudioFormatReaderStream::mp_SetGain( const double ac_dGain )
{
  const Lock L( mc_ReadLock );
  m_Output.mp_SetReadGain( ac_dGain );
}

const double& AudioFormatReaderStream::mf_dGetGain() const
{
  return m_Output.mf_dGetReadGain();
}

void AudioFormatReaderStream::mp_ReplaceReader( AudioFormatReader* const ac_pSource, const bool ac_bDeleteReader )
{
  PtrCheck( ac_pSource );
  Q_ASSERT( ac_pSource->mf_eBitMode() == m_pSource->mf_eBitMode() );
  Q_ASSERT( ac_pSource->mf_nChannels() == m_pSource->mf_nChannels() );
  const Lock L( mc_ReadLock );
  if( mv_bDeleteReader )
    delete m_pSource;
  m_pSource         = ac_pSource;
  mv_bDeleteReader  = ac_bDeleteReader;
  mv_bEOF           = false;
  mv_bOutputCleared = false;
}

const Stream& AudioFormatReaderStream::Read()
{
  const Lock L( mc_ReadLock );

    //check EOF
  if( mv_bEOF && mv_bOutputCleared == false )
  {
    m_Output.mp_Clear();
    mv_bOutputCleared = true;
  }

    //read
  if( !mv_bEOF )
  {
    const unsigned nSamples = m_Output.mf_nGetBufferSize();
    mv_nReadLastTime = m_Output.ReadFromSource( m_pSource, nSamples, mc_bNormalize );
    if( mv_nReadLastTime != nSamples )
      mv_bEOF = true;
  }

    //finito
  return m_Output;
}

AudioFormatWriterStream::AudioFormatWriterStream( AudioFormatWriter* const ac_cpSource, const unsigned ac_nBufferSize,
                                                  const bool ac_bNormalize, const bool ac_bClipping, const bool ac_bDeleteWriter ) :
    OutputStream(),
  m_pSource( ac_cpSource ),
  m_Output( ac_cpSource->mf_nChannels(), ac_nBufferSize ), //allocates buffer but is not always used
  mc_bNormalize( ac_bNormalize ),
  mc_bClipping( ac_bClipping ),
  mv_bEOF( false ),
  mv_bDeleteWriter( ac_bDeleteWriter ),
  mc_WriteLock()
{
}

AudioFormatWriterStream::~AudioFormatWriterStream()
{
  const Lock L( mc_WriteLock );
  if( mv_bDeleteWriter )
    delete m_pSource;
}

void AudioFormatWriterStream::Write( const Stream& ac_Stream )
{
  const Lock L( mc_WriteLock );

    //check EOF
  if( mv_bEOF )
    return;

    //copy channel pointers from input  FIXME no need to deepcopy this
  m_Output.mp_CopyFrom( ac_Stream );

    //write
  const unsigned nSamples = m_Output.mf_nGetBufferSize();
  if( m_Output.WriteToSource( m_pSource, nSamples, mc_bNormalize, mc_bClipping ) != nSamples )
    mv_bEOF = true;
}

void AudioFormatWriterStream::mp_ReplaceWriter( AudioFormatWriter* const ac_cpSource, const bool ac_bDeleteWriter )
{
  PtrCheck( ac_cpSource );
  Q_ASSERT( ac_cpSource->mf_eBitMode() == m_pSource->mf_eBitMode() );
  Q_ASSERT( ac_cpSource->mf_nChannels() == m_pSource->mf_nChannels() );
  const Lock L( mc_WriteLock );
  if( mv_bDeleteWriter )
    delete m_pSource;
  m_pSource         = ac_cpSource;
  mv_bDeleteWriter  = ac_bDeleteWriter;
  mv_bEOF           = false;
}

PositionableAudioFormatReaderStream::PositionableAudioFormatReaderStream( PositionableAudioFormatReader* const ac_pSource, const unsigned ac_nBufferSize,
                                                                         const bool ac_bNormalize, const bool ac_bDeleteReader ) :
  mc_bNormalize( ac_bNormalize ),
  mv_bDeleteReader( ac_bDeleteReader ),
  mv_bEOF( false ),
  mv_bOutputCleared( false ),
  mv_nReadLastTime( 0 ),
  mv_nLoops( 1 ),
  mv_nLooped( 0 ),
  m_Output( ac_pSource->mf_nChannels(), ac_nBufferSize ),
  m_pSource( PtrCheck( ac_pSource ) ),
  mc_ReadLock()
{
}

PositionableAudioFormatReaderStream::~PositionableAudioFormatReaderStream()
{
  const Lock L( mc_ReadLock );
  if( mv_bDeleteReader )
    delete m_pSource;
}

void PositionableAudioFormatReaderStream::mp_ReplaceReader( PositionableAudioFormatReader* const ac_pSource, const bool ac_bDeleteReader )
{
  PtrCheck( ac_pSource );
  Q_ASSERT( ac_pSource->mf_eBitMode() == m_pSource->mf_eBitMode() );
  Q_ASSERT( ac_pSource->mf_nChannels() == m_pSource->mf_nChannels() );
  const Lock L( mc_ReadLock );
  if( mv_bDeleteReader )
    delete m_pSource;
  m_pSource         = ac_pSource;
  mv_bDeleteReader  = ac_bDeleteReader;
  mv_bEOF           = false;
  mv_bOutputCleared = false;
}

const Stream& PositionableAudioFormatReaderStream::Read()
{
  const Lock L( mc_ReadLock );

    //check EOF
  if( mv_bEOF )
  {
    if( !mv_bOutputCleared )
    {
      m_Output.mp_Clear();
      mv_bOutputCleared = true;
    }
    return m_Output;
  }

    //read
  const unsigned nSamples = m_Output.mf_nGetBufferSize();
  const unsigned nLeft = m_pSource->mf_lSamplesLeft();
  mv_nReadLastTime = m_Output.ReadFromSource( m_pSource, nSamples, mc_bNormalize );

  if( mf_bIsLooping() )
  {
    if( nLeft < nSamples && //loop end passed
        ++mv_nLooped == mv_nLoops )
    {
      m_Output.mp_ClearRegion( nSamples - nLeft, nLeft );
      m_pSource->mp_SeekPosition( 0 );
      mv_bEOF = true;
    }
  }
  else if( mv_nReadLastTime != nSamples )
  {
    mv_bEOF = true;
  }
    //finito
  return m_Output;
}

INLINE unsigned long PositionableAudioFormatReaderStream::mf_lSampleRate() const
{
  return m_pSource->mf_lSampleRate();
}

INLINE unsigned long PositionableAudioFormatReaderStream::mf_lSamplesLeft() const
{
  return mf_lTotalSamples() - mf_lCurrentPosition();
}

INLINE unsigned long PositionableAudioFormatReaderStream::mf_lTotalSamples() const
{
  return mv_nLoops * m_pSource->mf_lTotalSamples();
}

INLINE unsigned long PositionableAudioFormatReaderStream::mf_lCurrentPosition() const
{
  if( mf_bIsLooping() )
    return ( mv_nLooped * m_pSource->mf_lTotalSamples() ) + m_pSource->mf_lCurrentPosition();
  return m_pSource->mf_lCurrentPosition();
}

void PositionableAudioFormatReaderStream::mp_SeekPosition( const unsigned long ac_nPosition )
{
  const Lock L( mc_ReadLock );

  if( mv_nLoops == 1 )
  {
    m_pSource->mp_SeekPosition( ac_nPosition );
    mv_nLooped = 0;
    mv_bEOF = false;
    mv_bOutputCleared = false;
  }
  else
  {
    const unsigned long nTotal = m_pSource->mf_lTotalSamples();
    const unsigned long nStrip = ac_nPosition % nTotal;
    const unsigned      nFMode = ac_nPosition / nTotal;
    m_pSource->mp_SeekPosition( nStrip );
    mv_bEOF = false;
    mv_bOutputCleared = false;
    mv_nLooped = nFMode;
  }
}

INLINE void PositionableAudioFormatReaderStream::mp_SetNumLoops( const unsigned ac_nLoops )
{
  const Lock L( mc_ReadLock );
  if( !mv_bEOF && ac_nLoops >= mv_nLoops )
  {
    if( ac_nLoops > 1 )
    {
      m_pSource->mp_SetLoopAll();
      mv_nLoops = ac_nLoops;
    }
  }
  else
    Q_ASSERT( 0 && "loop not set" );
}

INLINE unsigned PositionableAudioFormatReaderStream::mf_nGetNumLoops() const
{
  return mv_nLoops;
}

INLINE unsigned PositionableAudioFormatReaderStream::mf_nGetNumLooped() const
{
  return mv_nLooped;
}

INLINE bool PositionableAudioFormatReaderStream::mf_bIsLooping() const
{
  return mv_nLoops != 1;
}

void PositionableAudioFormatReaderStream::mp_SetGain( const double ac_dGain )
{
  const Lock L( mc_ReadLock );
  m_Output.mp_SetReadGain( ac_dGain );
}

const double& PositionableAudioFormatReaderStream::mf_dGetGain() const
{
  return m_Output.mf_dGetReadGain();
}
