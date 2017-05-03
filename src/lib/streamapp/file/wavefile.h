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

#include <QFile>
#include <QString>
#include <QScopedPointer>

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
      short channelCount;
      unsigned samplesPerSecond;
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
      riffHeader(new RIFF()),
      fmtHeader(new FMT() ),
      dataHeader(new DATA()),
      bytesPerSample(0),
      dataLeft(0),
      readOffset(0),
      bytesPerFrame(0)
    {}

      /**
        * The buffersize for the working buffer.
        * Keep this a multiple of 3!!
        */
    static const unsigned tempBufSize = 1440 * 8; //optimum for 1024 ac_nSamples, 2 channels  ??

      /**
        * The header size.
        */
    static const unsigned long waveHeaderSize = sizeof(RIFF) + sizeof(FMT) + sizeof(DATA);

      /**
        * Destructor.
        */
    ~WaveFileData()
    {
      delete  riffHeader;
      delete  fmtHeader;
      delete  dataHeader;
    }

  protected:
    RIFF* riffHeader;
    FMT*  fmtHeader;
    DATA* dataHeader;
    AudioFormat::mt_eBitMode m_eBitMode;
    unsigned short bytesPerSample;
    unsigned long dataLeft;
    unsigned long readOffset;
    unsigned long bytesPerFrame; //bytesPerSample * nChannels
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
        * @param fileName the file's path
        * @return false if the file cannot be opened
        */
    bool mp_bOpen(const QString& fileName);

      /**
        * Close the file.
        * @return false if the file is not open, or if there was an error closing it.
        */
    bool mp_bClose();

      /**
        * Implementation of the AudioFormat method.
        */
    unsigned long Read(void** a_pBuf, const unsigned ac_nSamples);

      /**
        * Implementation of the AudioFormat method.
        */
    unsigned mf_nChannels() const
    { return fmtHeader->fmtFORMAT.channelCount; }

      /**
        * Implementation of the AudioFormat method.
        */
    unsigned long mf_lSampleRate() const
    { return fmtHeader->fmtFORMAT.samplesPerSecond; }

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
    void mp_SeekPosition(const unsigned long ac_nPosition);

      /**
        * Implementation of the PositionableAudioFormatReader method.

    bool mp_bSetLoopStart(const unsigned long ac_lPosition);*/

      /**
        * Implementation of the PositionableAudioFormatReader method.

    bool mp_bSetLoopEnd(const unsigned long ac_lPosition);*/

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

    void mf_Convert(int*& dest0, int*& dest1, const unsigned nSamplesRead);

      /**
        * Checks if four byte data matches the specified characters.
        * @param idPar pointer to the data buffer
        * @param A the first character
        * @param B the second character
        * @param C the third character
        * @param D the fourth character
        * @return
        */
    bool CheckID (char *idPar, char A, char B, char C, char D);

    typedef containers::LoopLogic< unsigned long, true > mt_LoopLogic;

    unsigned long   totalSampleCount;  //keep as member to avoid calculating it everytime
    QScopedPointer<QFile> inputFile;
    mt_LoopLogic*   loopLogic;
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
        * @param fileName the file's path
        * @param channelCount the number of channels
        * @param ac_lFs the samplerate
        * @param ac_eMode the bitmode to use
        * @return false if the file cannot be opened or if the parameters are not supported by the format
        */
    bool mp_bOpen( const QString& fileName,
                   const unsigned channelCount, const unsigned long ac_lFs, const AudioFormat::mt_eBitMode ac_eMode = AudioFormat::MSBint16 );

      /**
        * Close the file.
        * @return false if the file is not open.
        */
    bool mp_bClose();

      /**
        * Implementation of the AudioFormatWriter method.
        */
    unsigned long Write(const void** a_pBuf, const unsigned ac_nSamples);

      /**
        * Implementation of the AudioFormat method.
        */
    unsigned mf_nChannels() const
    { return fmtHeader->fmtFORMAT.channelCount; }

      /**
        * Implementation of the AudioFormat method.
        */
    unsigned long mf_lSampleRate() const
    { return fmtHeader->fmtFORMAT.samplesPerSecond; }

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

    QScopedPointer<QFile> outputFile;
  };

}

#endif //#ifndef __WAVEFILE_H_
