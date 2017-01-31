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

#include "streamappfactory.h"
#include "stream.h"
#include "audioformatstream.h"
#include "file/wavefile.h"
#include "soundcard/soundcardfactory.h"
#include "utils/stringexception.h"
#include "appcore/deleter.h"
#include "silentreader.h"

using namespace utils;
using namespace streamapp;

Q_LOGGING_CATEGORY(APEX_SA, "apex.streamapp")

StreamAppFactory::StreamAppFactory() :
  mv_nChannels        (  2         ),
  mv_bNormalize       (  true     ),
  mv_bDeleteSources   (  true      ),
  mv_nBufferSize      (  512       ),
  mv_nSampleRate      (  44100     ),
//  mv_bAllowResampling (  false     ),
  mv_eBitMode         (  AudioFormat::MSBint16   )
{

}

StreamAppFactory::~StreamAppFactory()
{

}

PositionableAudioFormatReader* StreamAppFactory::mf_pReader( const std::string& ac_sName )
{
  InputWaveFile* p = new InputWaveFile();
  if( !p->mp_bOpen( ac_sName ) )
  {
    delete p;
    throw StringException( "InputWaveFile : couldn't open " + ac_sName );
  }
  return p;
}

PositionableInputStream*  StreamAppFactory::mf_pReaderStream( const std::string& ac_sName, const unsigned ac_nBufferSize )
{
  PositionableAudioFormatReader* p = mf_pReader( ac_sName );
  return new PositionableAudioFormatReaderStream( p, ac_nBufferSize, mv_bNormalize, mv_bDeleteSources );
}

PositionableInputStream* StreamAppFactory::mf_pReaderStream( PositionableAudioFormatReader* ac_Reader, const unsigned ac_nBufferSize )
{
  return new PositionableAudioFormatReaderStream( ac_Reader, ac_nBufferSize, mv_bNormalize, mv_bDeleteSources );
}

InputStream* StreamAppFactory::mf_pReaderStream( AudioFormatReader* ac_Reader, const unsigned ac_nBufferSize )
{
  return new AudioFormatReaderStream( ac_Reader, ac_nBufferSize, mv_bNormalize, mv_bDeleteSources );
}

PositionableInputStream* StreamAppFactory::mf_pSilentStream(  const unsigned ac_nChan, const unsigned long ac_lFs, const unsigned ac_nSize, const unsigned long ac_lLength )
{
  return mf_pReaderStream( new SilentReader( ac_nChan, ac_lFs, ac_lLength ), ac_nSize );
}

/*PositionableInputStream*  StreamAppFactory::mf_pWaveFileIn( const std::string& ac_sName, const unsigned , const unsigned long, const unsigned ac_nBufferSize )
{
  InputWaveFile* p = new InputWaveFile();
  if( !p->mp_bOpen( ac_sName ) )
  {
    delete p;
    throw StringException( "InputWaveFile : couldn't open " + ac_sName );
  }
  return new PositionableAudioFormatReaderStream( p, ac_nBufferSize, mv_bNormalize, true );
}

OutputStream*             StreamAppFactory::mf_pWriterStream( const std::string& ac_sName )
{
  OutputWaveFile* p = new OutputWaveFile();
  if( !p->mp_bOpen( ac_sName, mv_nChannels, mv_nSampleRate, mv_eBitMode ) )
  {
    delete p;
    throw StringException( "OutputWaveFile : couldn't open " + ac_sName );
  }
  return new AudioFormatWriterStream( p, mv_nBufferSize, mv_bNormalize, true );
}*/

AudioFormatWriter* StreamAppFactory::mf_pWriter( const std::string& ac_sName, const unsigned ac_nChannels, const unsigned long ac_lSampleRate, const AudioFormat::mt_eBitMode ac_eBitMode )
{
  OutputWaveFile* p = new OutputWaveFile();
  if( !p->mp_bOpen( ac_sName, ac_nChannels, ac_lSampleRate, ac_eBitMode ) )
  {
    delete p;
    throw StringException( "OutputWaveFile : couldn't open " + ac_sName );
  }
  return p;
}

OutputStream* StreamAppFactory::mf_pWriterStream( const std::string& ac_sName, const unsigned ac_nChannels, const unsigned ac_nSampleRate, const unsigned ac_nBufferSize, const AudioFormat::mt_eBitMode ac_eBitMode )
{
  AudioFormatWriter* p = mf_pWriter( ac_sName, ac_nChannels, ac_nSampleRate, ac_eBitMode );
  return new AudioFormatWriterStream( p, ac_nBufferSize, mv_bNormalize, false, true );
}

ISoundCard*               StreamAppFactory::mf_pSoundCard( const gt_eDeviceType ac_eType , const std::string& ac_sDriverName )
{
  std::string sErr;
  ISoundCard* p = SoundCardFactory::CreateSoundCard( ac_sDriverName, ac_eType, sErr );
  if( !p )
    throw StringException( "ISoundCard : couldn't open " + ac_sDriverName + "reason : " + sErr );
  return p;
}

ISoundCard*               StreamAppFactory::mf_pSoundCard( const gt_eDeviceType ac_eType , const std::string& ac_sDriverName,
                                                           const unsigned ac_nInputChannels, const unsigned ac_nOutputChannels,
                                                           const unsigned long ac_nSampleRate, const unsigned ac_nBufferSize  )
{
  ISoundCard* p = mf_pSoundCard( ac_eType, ac_sDriverName );
  if( !p->mp_bOpenDriver( ac_nInputChannels, ac_nOutputChannels, ac_nSampleRate, ac_nBufferSize ) )
  {
    const std::string c_sError( p->mf_sGetLastError() );
    delete p;
    throw StringException( "ISoundCard : couldn't open " + ac_sDriverName + ", soundcard error = " + c_sError );
  }
  return p;
}

InputStream*              StreamAppFactory::mf_pSoundCardIn( const ISoundCard& ac_SoundCard )
{
  return new AudioFormatReaderStream( ac_SoundCard.mf_pCreateReader(), ac_SoundCard.mf_nGetBufferSize(), mv_bNormalize, true );
}

OutputStream*             StreamAppFactory::mf_pSoundCardOut( const ISoundCard& ac_SoundCard )
{
  return new AudioFormatWriterStream( ac_SoundCard.mf_pCreateWriter(), ac_SoundCard.mf_nGetBufferSize(), mv_bNormalize, true );
}
