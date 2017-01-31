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
 
#ifndef __STREAMAPPFACTORY_H__
#define __STREAMAPPFACTORY_H__

#include <string>
#include "typedefs.h"
#include "audioformat.h"
#include "factorytypes.h"
#include "appcore/singleton.h"

namespace streamapp
{
  class InputWaveFile;
  class OutputWaveFile;
  class AudioFormatReader;
  class AudioFormatWriter;
  class PositionableAudioFormatReader;
  class InputStream;
  class OutputStream;
  class AudioFormatReaderStream;
  class AudioFormatWriterStream;
  class PositionableInputStream;
  class ISoundCard;

    /**
      * StreamAppFactory
      *   used to get the different streams and soundcards, throws StringExceptions.
      *   'rules' can be set and StreamAppFactory will throw an
      *   exception with the error's description, when these can not be met
      *   The rules can be bypassed one by one through choosing the appropriate method.
      *   Doesn't delete resources.
      ********************************************************************************* */
  class StreamAppFactory : public appcore::Singleton<StreamAppFactory>
  {
    friend class appcore::Singleton<StreamAppFactory>;
  public:
    ~StreamAppFactory();

      //!following methods do not use any of the rules except for normalizing and deleting sources
      //!only PCM Wave Files are supported
    PositionableAudioFormatReader*  mf_pReader( const std::string& ac_sName );

    PositionableInputStream*        mf_pReaderStream( const std::string& ac_sName, const unsigned ac_nBufferSize );
    PositionableInputStream*        mf_pReaderStream( PositionableAudioFormatReader* a_pReader, const unsigned ac_nBufferSize );
    InputStream*                    mf_pReaderStream( AudioFormatReader* a_pReader, const unsigned ac_nBufferSize );

    AudioFormatWriter*              mf_pWriter( const std::string& ac_sName, const unsigned ac_nChannels, const unsigned long ac_lSampleRate, const AudioFormat::mt_eBitMode ac_eBitMode = AudioFormat::MSBint16 );

    OutputStream*                   mf_pWriterStream( const std::string& ac_sName, const unsigned ac_nChannels, const unsigned ac_lSampleRate, const unsigned ac_nBufferSize, const AudioFormat::mt_eBitMode ac_eBitMode = AudioFormat::MSBint16 );
    OutputStream*                   mf_pWriterStream( AudioFormatWriter* a_pWriter, const unsigned ac_nBufferSize );

    ISoundCard*                     mf_pSoundCard( const gt_eDeviceType ac_eType , const std::string& ac_sDriverName = sc_sDefault.toStdString() ); //!doesn't open soundcard
    ISoundCard*                     mf_pSoundCard( const gt_eDeviceType ac_eType , const std::string& ac_sDriverName,               //!opens soundcard
                                                   const unsigned ac_nInputChannels, const unsigned ac_nOutputChannels,
                                                   const unsigned long ac_lSampleRate, const unsigned ac_nBufferSize );

      //!buffersize is bound to soundcard itself
    InputStream*                    mf_pSoundCardIn( const ISoundCard& ac_SoundCard );
    OutputStream*                   mf_pSoundCardOut( const ISoundCard& ac_SoundCard );

    PositionableInputStream*        mf_pSilentStream( const unsigned ac_nChan, const unsigned long ac_lFs, const unsigned ac_nSize, const unsigned long ac_lLength );

    /*PositionableInputStream*  mf_pWaveFileIn( const std::string& ac_sName, const unsigned ac_nChannels );
    PositionableInputStream*  mf_pWaveFileIn( const std::string& ac_sName, const unsigned ac_nChannels, const unsigned long ac_lSampleRate );
    PositionableInputStream*  mf_pWaveFileIn( const std::string& ac_sName, const unsigned ac_nChannels, const unsigned long ac_lSampleRate, const unsigned ac_nBufferSize );*/
      //  OutputStream*             mf_pWaveFileOut( const std::string& ac_sName );
    OutputStream*             mf_pWaveFileOut( const std::string& ac_sName, const unsigned ac_nChannels );
    //    OutputStream*             mf_pWaveFileOut( const std::string& ac_sName, const unsigned ac_nChannels, const unsigned ac_lSampleRate );
    //    OutputStream*             mf_pWaveFileOut( const std::string& ac_sName, const unsigned ac_nChannels, const unsigned ac_lSampleRate );

      //!scale samples to -1.0 - +1.0 range
    void mp_SetNormalizeRule  ( const bool          ac_bNormalize       = true  )
    { mv_bNormalize = ac_bNormalize; }

      //!specify if streams should delete their reader/writer
      //!better leave this true unless you're sure not to forget to delete them
    void mp_SetDeletionRule   ( const bool          ac_bDeleteSources   = true  )
    { mv_bDeleteSources = ac_bDeleteSources; }

    void mp_SetChannelRule    ( const unsigned      ac_nChannels        = 2     )
    { mv_nChannels = ac_nChannels; }
    void mp_SetBufferSizeRule ( const unsigned      ac_nBufferSize      = 512   )
    { mv_nBufferSize = ac_nBufferSize; }
    void mp_SetSampleRateRule ( const unsigned long ac_nSampleRate      = 44100 ) //resampling not implemented yet; use buffer so size matches
    {
      mv_nSampleRate = ac_nSampleRate;
      //mv_bAllowResampling = ac_bAllowResampling;
    }
    void mp_SetBitModeRule    ( const AudioFormat::mt_eBitMode ac_eBitMode = AudioFormat::MSBint16 )
    { mv_eBitMode = ac_eBitMode; }


  protected:
    StreamAppFactory();

  private:
    unsigned      mv_nChannels;
    bool          mv_bNormalize;
    bool          mv_bDeleteSources;
    unsigned      mv_nBufferSize;
    unsigned long mv_nSampleRate;
    //bool          mv_bAllowResampling;
    AudioFormat::mt_eBitMode mv_eBitMode;
  };

}

#endif //#ifndef __STREAMAPPFACTORY_H__
