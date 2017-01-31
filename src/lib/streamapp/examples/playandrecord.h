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
 
#ifndef __STREAMAPPS_H__
#define __STREAMAPPS_H__

#include <string>
#include "factorytypes.h"
#include "audioformatstream.h"
#include "callback/multicallback.h"
#include "callback/streamappcallbacks.h"

namespace streamapp
{
  class EofCheck;
  class ISoundCard;
  class IOProcessor2;
  class MultiCallback;
  class AudioFormatReader;
  class AudioFormatWriter;
  class AudioFormatReaderStream;
  class PositionableAudioFormatReader;

    /**
      * StreamAppBase
      *   NOT FINISHED
      **************** */
  class StreamAppBase
  {
  public:
    StreamAppBase( const unsigned ac_nBufferSize );
    ~StreamAppBase();

    void mp_SetSoundCard( const gt_eDeviceType ac_eType, const std::string& ac_sDriverName );
    void mp_OpenSoundCard( const unsigned ac_nInputChannels, const unsigned ac_nOutputChannels, const unsigned long ac_lSampleRate );
    void mp_UseSoundCardAsInput();
    void mp_UseSoundCardAsOutput();

    void mp_AddInput( AudioFormatReader* a_pInput );
    void mp_AddInput( PositionableAudioFormatReader* a_pInput );
    void mp_AddInput( AudioFormatReaderStream* a_pInput );
    void mp_AddInput( PosAudioFormatStream* a_pInput );
    void mp_AddOutput( AudioFormatWriter* a_pOutput );
    void mp_AddProcessor( IStreamProcessor* a_pProcessor );

    void mp_Start( const bool ac_bAutoStop = true );
    void mp_Stop();
    void mp_ResetPosition();
    bool mf_bIsRunning() const;

    void mp_RegisterEofCallback( Callback& a_Callback ); //can only be one

  private:
    const unsigned          mc_nBufferSize;
    ISoundCard*             m_pCard;
    EofCheck*      const    mc_pChecker;
    MultiCallback* const    mc_pCallback;
    StreamAppChannel* const mc_pChannel;

    StreamAppBase( const StreamAppBase& );
    StreamAppBase& operator = ( const StreamAppBase& );
  };

    /**
      * PlaySound
      *   class like windows' PlaySound function.
      *   Tries to open the default stereo device with the samplerate
      *   from the file, throws StringException on failure.
      *   Mono files will play as two identical stereo channels
      *************************************************************** */
  class PlaySound
  {
  public:
      /**
        * Constructor.
        * Initializes soundcard.
        */
    PlaySound();

      /**
        * Destructor.
        */
    ~PlaySound();

      /**
        * Play a file until it's done.
        * @param ac_sWaveFile the file path
        */
    void mp_Play( const std::string& ac_sWaveFile );

  private:
    StreamAppBase m_Base;

    PlaySound( const PlaySound& );
    PlaySound& operator = ( const PlaySound& );
  };

      /**
        * PlayAndRecord
        *   simple play and record with soundcard.
        *   Plays until inputfile is read completely,
        *   then closes outputfile
        ********************************************* */
  class PlayAndRecord
  {
  public:
      /**
        * Constructor.
        * @param ac_nChannels the number of channels to use
        * @param ac_nBufferSize the buffer size
        * @param ac_lSampleRate the samplerate
        */
    PlayAndRecord( const unsigned ac_nChannels, const unsigned ac_nBufferSize, const unsigned long ac_lSampleRate );

      /**
        * Destructor.
        */
    ~PlayAndRecord();

      /**
        * Set which soundcard to use.
        * @param ac_eType the type
        * @param ac_sDriverName the driver name
        */
    void mp_SetSoundcard( const gt_eDeviceType ac_eType = PORTAUDIO, const std::string& ac_sDriverName = sc_sDefault.toStdString() );

      /**
        * Set the file to play.
        * @param ac_sFile the file's path
        */
    void mp_SetFileToPlay( const std::string& ac_sFile );

      /**
        * Set file position.
        * @param ac_lPosition the position
        */
    void mp_SetPlayPosition( const unsigned long ac_lPosition );

      /**
        * Set the file to record.
        * Closes current file if any.
        * @param ac_sFile the file's path
        */
    void mp_SetFileToRecord( const std::string& ac_sFile );

      /**
        * Start playing and recording.
        * @param ac_bWaitUntilDone if true, it's a blocking call
        */
    void mp_Start( const bool ac_bWaitUntilDone = true );

      /**
        * Stop playing.
        */
    void mp_Stop();

      /**
        * Run callback in while loop until input is read.
        */
    void mp_Manual();

      /**
        * Set the callback to use when input is done.
        * Use this if not using ac_bWaitUntilDone, in order
        * to know when you're finished.
        * @param acpCallback
        */
    void mp_ReplaceEofCallback( Callback& acpCallback );

  private:
    const unsigned            mc_nChannels;
    const unsigned            mc_nBufferSize;
    const unsigned long       mc_lSampleRate;
    ISoundCard*               m_pCard;
    EofCheck*                 m_pCheck;
    Channel*                  m_pCallback;
    MultiCallback*            m_pMasterCallback;
    IOProcessor2*             m_pProcessor;
    PosAudioFormatStream*     m_pInput;
    AudioFormatWriterStream*  m_pOutput;

    PlayAndRecord( const PlayAndRecord& );
    PlayAndRecord& operator = ( const PlayAndRecord& );
  };

}

#endif //#ifndef __STREAMAPPS_H__
