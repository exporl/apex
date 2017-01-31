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

#ifndef __WAVEFILE_H_
#define __WAVEFILE_H_

#include "../audioformat.h"

#include <fstream>

  //!define a WORD
#ifdef WORD
#undef WORD
#endif
typedef unsigned short WORD;

namespace str3amapp
{
  namespace containers
  {
    template< class tType, bool tUseRef >
    class LoopLogic;
  }
}
using namespace str3amapp;

namespace streamapp
{

  /**
  * WaveFileData
  *   class with all internal data for both input and output wavefiles.
  ********************************************************************* */
  class WaveFileData
  {
  public:
      /**
        * WAVEFORM
        ********** */
    struct WAVEFORM
    {
      short wFormatTag;
      short nChannels;
      unsigned nSamplesPerSec;
      unsigned nAvgBytesPerSec;
      short nBlockAlign;
      short wBitsPerSample;
    };

      /**
        * RIFF
        ****** */
    struct RIFF
    {
      char riffID[4];
      unsigned riffSIZE;
      char riffFORMAT[4];
    };

      /**
        * FMT
        ***** */
    struct FMT
    {
      char fmtID[4];
      unsigned fmtSIZE;
      WAVEFORM fmtFORMAT;
    };

      /**
        * DATA
        ****** */
    struct DATA
    {
      char dataID[4];
      unsigned dataSIZE;
    };

      /**
        * Constructor.
        */
    WaveFileData() :
      m_pRIFF( new RIFF() ),
      m_pFMT( new FMT()  ),
      m_pDATA( new DATA() ),
      m_nBytesPerSample( 0 ),
      m_nDataLeft( 0 ),
      m_nReadOffset( 0 ),
      m_nBytesPerFrame( 0 )
    {}

      /**
        * The buffersize for the working buffer.
        * Keep this a multiple of 3!!
        */
    static const unsigned tempBufSize = 1440 * 8; //optimum for 1024 ac_nSamples, 2 channels  ??

      /**
        * The header size.
        */
    static const unsigned long sf_lHeaderSize = sizeof( RIFF ) + sizeof( FMT ) + sizeof( DATA );

      /**
        * Destructor.
        */
    ~WaveFileData()
    {
      delete  m_pRIFF;
      delete  m_pFMT;
      delete  m_pDATA;
    }

  protected:
    RIFF* m_pRIFF;
    FMT*  m_pFMT;
    DATA* m_pDATA;
    AudioFormat::mt_eBitMode m_eBitMode;
    unsigned short m_nBytesPerSample;
    unsigned long m_nDataLeft;
    unsigned long m_nReadOffset;
    unsigned long m_nBytesPerFrame; //m_nBytesPerSample * nChannels
    char tempBuffer[ tempBufSize ];
  };

    /**
      * InputWaveFile
      *   PositionableAudioFormatReader implementation for linear pcm wavefiles (Microsoft).
      *   @see OutputWaveFile
      ************************************************************************************** */
  class InputWaveFile : public PositionableAudioFormatReader,
                        private WaveFileData
  {
  public:
      /**
        * Constructor.
        */
    InputWaveFile();

      /**
        * Destructor.
        */
    ~InputWaveFile();

      /**
        * Open a file for reading.
        * @param ac_sFileName the file's path
        * @return false if the file cannot be opened
        */
    bool mp_bOpen( const std::string& ac_sFileName );

      /**
        * Close the file.
        * @return false if the file is not open, or if there was an error closing it.
        */
    bool mp_bClose();

      /**
        * Implementation of the AudioFormat method.
        */
    unsigned long Read( void** a_pBuf, const unsigned ac_nSamples );

      /**
        * Implementation of the AudioFormat method.
        */
    unsigned mf_nChannels() const
    { return m_pFMT->fmtFORMAT.nChannels; }

      /**
        * Implementation of the AudioFormat method.
        */
    unsigned long mf_lSampleRate() const
    { return m_pFMT->fmtFORMAT.nSamplesPerSec; }

      /**
        * Implementation of the AudioFormat method.
        */
    AudioFormat::mt_eBitMode mf_eBitMode() const
    { return m_eBitMode; }

      /**
        * Implementation of the PositionableAudioFormatReader method.
        */
    unsigned long mf_lSamplesLeft() const;

      /**
        * Implementation of the PositionableAudioFormatReader method.
        */
    unsigned long mf_lTotalSamples() const;

      /**
        * Implementation of the PositionableAudioFormatReader method.
        */
    unsigned long mf_lCurrentPosition() const;

      /**
        * Implementation of the PositionableAudioFormatReader method.
        */
    void mp_SeekPosition( const unsigned long ac_nPosition );

      /**
        * Implementation of the PositionableAudioFormatReader method.

    bool mp_bSetLoopStart( const unsigned long ac_lPosition );*/

      /**
        * Implementation of the PositionableAudioFormatReader method.

    bool mp_bSetLoopEnd( const unsigned long ac_lPosition );*/

  private:
      /**
        * Reads the RIFF chunk.
        */
    void ReadRIFF();

      /**
        * Reads the FMT chunk.
        */
    void ReadFMT ();

    /**
      * Reads the FMT chunk.
      */
    void ReadDATA();

    void mf_Convert( int*& dest0, int*& dest1, const unsigned ac_nChannels );

      /**
        * Checks if four byte data matches the specified characters.
        * @param idPar pointer to the data buffer
        * @param A the first character
        * @param B the second character
        * @param C the third character
        * @param D the fourth character
        * @return
        */
    bool CheckID ( char *idPar, char A, char B, char C, char D );

    typedef containers::LoopLogic< unsigned long, true > mt_LoopLogic;

    unsigned long   m_lTotalSamples;  //keep as member to avoid calculating it everytime
    WaveFileData    m_Data;
    std::ifstream*  m_pif;
    mt_LoopLogic*   m_pLoop;
  };

    /**
      * OutputWaveFile
      *   AudioFormatWriter implementation for linear pcm wavefiles.
      *   @see OutputWaveFile
      ************************************************************** */
  class OutputWaveFile : public AudioFormatWriter,
                         private WaveFileData
  {
  public:
    OutputWaveFile();
    ~OutputWaveFile();

      /**
        * Open a file for writing.
        * Replaces the file if it exists already, else creates a new one.
        * @param ac_sFileName the file's path
        * @param ac_nChannels the number of channels
        * @param ac_lFs the samplerate
        * @param ac_eMode the bitmode to use
        * @return false if the file cannot be opened or if the parameters are not supported by the format
        */
    bool mp_bOpen( const std::string& ac_sFileName,
                   const unsigned ac_nChannels, const unsigned long ac_lFs, const AudioFormat::mt_eBitMode ac_eMode = AudioFormat::MSBint16 );

      /**
        * Close the file.
        * @return false if the file is not open.
        */
    bool mp_bClose();

      /**
        * Implementation of the AudioFormatWriter method.
        */
    unsigned long Write( const void** a_pBuf, const unsigned ac_nSamples );

      /**
        * Implementation of the AudioFormat method.
        */
    unsigned mf_nChannels() const
    { return m_pFMT->fmtFORMAT.nChannels; }

      /**
        * Implementation of the AudioFormat method.
        */
    unsigned long mf_lSampleRate() const
    { return m_pFMT->fmtFORMAT.nSamplesPerSec; }

      /**
        * Implementation of the AudioFormat method.
        */
    AudioFormat::mt_eBitMode mf_eBitMode() const
    { return m_eBitMode; }

  private:
      /**
        * Writes the wave header.
        */
    void WriteHeader();

    std::ofstream* m_pof;
  };

}

#endif //#ifndef __WAVEFILE_H_
