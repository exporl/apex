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
 
#include "playandrecord.h"
#include "streamappfactory.h"
#include "file/wavefile.h"
#include "soundcard/soundcard.h"
#include "streamappfactory.h"
#include "processors/processor.h"
#include "appcore/threads/thread.h"
#include "utils/stringexception.h"
#include "callback/manualcallbackrunner.h"

using namespace utils;
using namespace appcore;
using namespace streamapp;

StreamAppBase::StreamAppBase( const unsigned ac_nSize ) :
  mc_nBufferSize( ac_nSize ),
  m_pCard( 0 ),
  mc_pChecker( new EofCheck() ),
  mc_pCallback( new MultiCallback( true ) ),
  mc_pChannel( new StreamAppChannel() )
{
  mc_pCallback->mp_AddItem( mc_pChannel );
  mc_pCallback->mp_AddItem( mc_pChecker );
}

StreamAppBase::~StreamAppBase()
{
  delete mc_pCallback;  //deletes all
}

void StreamAppBase::mp_SetSoundCard( const gt_eDeviceType ac_eType, const std::string& ac_sDriverName )
{
  m_pCard = StreamAppFactory::sf_pInstance()->mf_pSoundCard( ac_eType, ac_sDriverName );
}

void StreamAppBase::mp_OpenSoundCard( const unsigned ac_nInputChannels, const unsigned ac_nOutputChannels, const unsigned long ac_lSampleRate )
{
  assert( m_pCard );
  if( m_pCard->mf_bIsOpen() )
    m_pCard->mp_bCloseDriver();
  if( !m_pCard->mp_bOpenDriver( ac_nInputChannels, ac_nOutputChannels, ac_lSampleRate, mc_nBufferSize ) )
    throw StringException( "Couldn't open Soundcard!!" );
}

void StreamAppBase::mp_UseSoundCardAsInput()
{
  assert( m_pCard );
  mc_pChannel->mp_AddInputStream( StreamAppFactory::sf_pInstance()->mf_pSoundCardIn( *m_pCard ) );
}

void StreamAppBase::mp_UseSoundCardAsOutput()
{
  assert( m_pCard && m_pCard->mf_bIsOpen() );
  mc_pChannel->mp_AddOutputStream( StreamAppFactory::sf_pInstance()->mf_pSoundCardOut( *m_pCard ) );
}

void StreamAppBase::mp_AddInput( AudioFormatReader* a_pInput )
{
  mp_AddInput( new AudioFormatReaderStream( a_pInput, mc_nBufferSize, false, true ) );
}

void StreamAppBase::mp_AddInput( PositionableAudioFormatReader* a_pInput )
{
  PositionableAudioFormatReaderStream* p = new PositionableAudioFormatReaderStream( a_pInput, mc_nBufferSize, false, true );
  mp_AddInput( p );
}

void StreamAppBase::mp_AddInput( AudioFormatReaderStream* a_pInput )
{
  mc_pChannel->mp_AddInputStream( a_pInput );
}

void StreamAppBase::mp_AddInput( PosAudioFormatStream* a_pInput )
{
  mc_pChannel->mp_AddInputStream( a_pInput );
  mc_pChecker->mp_RemoveAllInputs();
  mc_pChecker->mp_AddInputToCheck( *a_pInput );
}

void StreamAppBase::mp_AddOutput( AudioFormatWriter* a_pOutput )
{
  mc_pChannel->mp_AddOutputStream( new AudioFormatWriterStream( a_pOutput, mc_nBufferSize, false, false, true ) );
}

void StreamAppBase::mp_AddProcessor( IStreamProcessor* a_pProcessor )
{
  mc_pChannel->mp_AddProcessor( a_pProcessor );
}

void StreamAppBase::mp_Start( const bool ac_bAutoStop )
{
  if( m_pCard )
  {
    m_pCard->mp_bStart( *mc_pCallback );
    if( ac_bAutoStop )
    {
      while( mc_pChecker->mf_lSamplesLeft() )
        IThread::sf_Sleep( 10 );
      m_pCard->mp_bStop();
    }
  }
  else
  {
    ManualCallbackRunner runner( *mc_pChecker );
    runner.mp_SetPeriod( 10 );
    runner.mp_bStart( *mc_pChannel );
  }
}

void StreamAppBase::mp_Stop()
{
  if( m_pCard )
    m_pCard->mp_bStop();
}

void StreamAppBase::mp_ResetPosition()
{
  mc_pChecker->mp_ResetState();
  mc_pChecker->mp_SeekAll( 0 );
}

bool StreamAppBase::mf_bIsRunning() const
{
  if( m_pCard )
    return m_pCard->mf_bIsRunning();
  return false;
}

void StreamAppBase::mp_RegisterEofCallback( Callback& a_Callback )
{
  mc_pChecker->mp_InstallCallback( a_Callback );
}

PlaySound::PlaySound() :
  m_Base( 2048 )
{
  m_Base.mp_SetSoundCard( PORTAUDIO, sc_sDefault.toStdString() );
}

PlaySound::~PlaySound()
{

}

void PlaySound::mp_Play( const std::string& ac_sWaveFile )
{
  PositionableAudioFormatReader* p = StreamAppFactory::sf_pInstance()->mf_pReader( ac_sWaveFile );
  m_Base.mp_OpenSoundCard( 0, 2, p->mf_lSampleRate() );
  m_Base.mp_UseSoundCardAsOutput();
  m_Base.mp_AddInput( p );
  m_Base.mp_Start();
}

PlayAndRecord::PlayAndRecord( const unsigned ac_nChannels, const unsigned ac_nBufferSize, const unsigned long ac_lSampleRate ) :
  mc_nChannels( ac_nChannels ),
  mc_nBufferSize( ac_nBufferSize ),
  mc_lSampleRate( ac_lSampleRate ),
  m_pCard( 0 ),
  m_pCheck( new EofCheck() ),
  m_pCallback( new Channel( true ) ),
  m_pMasterCallback( new MultiCallback( true ) ),
  m_pProcessor( new IOProcessor2() ),
  m_pInput( 0 ),
  m_pOutput( 0 )
{
  m_pCallback->mp_SetProcessor( m_pProcessor );
  m_pMasterCallback->mp_AddItem( m_pCallback );
  m_pMasterCallback->mp_AddItem( m_pCheck );
}

PlayAndRecord::~PlayAndRecord()
{
  delete m_pCard;
  delete m_pMasterCallback; //also deletes m_pCallback and it's contents
  delete m_pInput;
  delete m_pOutput;
}

void PlayAndRecord::mp_SetSoundcard( const gt_eDeviceType ac_eType, const std::string& ac_sDriverName )
{
  if( !m_pCard )
  {
    StreamAppFactory* pFac = StreamAppFactory::sf_pInstance();
    m_pCard = pFac->mf_pSoundCard( ac_eType, ac_sDriverName, mc_nChannels, 2, mc_lSampleRate, mc_nBufferSize );
    m_pCallback->mp_SetSource( pFac->mf_pSoundCardIn( *m_pCard ) );
    m_pCallback->mp_SetSink( pFac->mf_pSoundCardOut( *m_pCard ) );
  }
  else
    throw utils::StringException( "Soundcard already set" );
}

void PlayAndRecord::mp_SetFileToPlay( const std::string& ac_sFile )
{
  if( !m_pInput )
  {
    m_pInput = (PosAudioFormatStream*) StreamAppFactory::sf_pInstance()->mf_pReaderStream( ac_sFile, mc_nBufferSize );
    m_pCheck->mp_AddInputToCheck( *m_pInput );
    m_pProcessor->mp_ReplaceReader( m_pInput );
  }
  else
  {
    m_pInput->mp_ReplaceReader( StreamAppFactory::sf_pInstance()->mf_pReader( ac_sFile ), true );
  }
    //this check is a bit redundant: m_pInput will already complin if #channels doesn't match
  /*if( m_pInput->mf_nGetNumChannels() != mc_nChannels || m_pInput->mf_pGetReader()->mf_lSampleRate() != mc_lSampleRate )
    throw utils::StringException( "Bad Iputfile" ); */
}

void PlayAndRecord::mp_SetPlayPosition( const unsigned long ac_lPosition )
{
  if( m_pInput )
  {
    m_pCheck->mp_SeekAll( ac_lPosition );
    /*if( m_pCheck->mf_lSamplesLeft() == 0 ) //finished already?
    {
      m_pCheck->mp_AddInputToCheck( *m_pInput );
    } */
  }
}

void PlayAndRecord::mp_SetFileToRecord( const std::string& ac_sFile )
{
  if( !m_pOutput )
  {
    m_pOutput = (AudioFormatWriterStream*) StreamAppFactory::sf_pInstance()->mf_pWriterStream( ac_sFile, mc_nChannels, mc_lSampleRate, mc_nBufferSize );
    m_pProcessor->mp_ReplaceWriter( m_pOutput );
  }
  else
  {
    m_pOutput->mp_ReplaceWriter( StreamAppFactory::sf_pInstance()->mf_pWriter( ac_sFile, mc_nChannels, mc_lSampleRate ), true );
  }
}

void PlayAndRecord::mp_Start( const bool ac_bWaitUntilDone )
{
  if( m_pCard )
  {
    m_pCard->mp_bStart( *m_pMasterCallback );
    if( ac_bWaitUntilDone )
    {
      while( m_pCheck->mf_lSamplesLeft() )
        IThread::sf_Sleep( 20 );
      mp_Stop();
    }
  }
}

void PlayAndRecord::mp_Manual()
{
  ManualCallbackRunner runner( *m_pCheck );
  runner.mp_bStart( *m_pCallback ); //no need to run master: ManualCallbackRunner will call m_pCheck's callback
}

void PlayAndRecord::mp_Stop()
{
  if( m_pCard )
    m_pCard->mp_bStop();
}

void PlayAndRecord::mp_ReplaceEofCallback( Callback& ac_Callback )
{
  m_pCheck->mp_InstallCallback( ac_Callback );
}
