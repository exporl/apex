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
 
#ifndef __MULTIWAVEFILE_H__
#define __MULTIWAVEFILE_H__

#include "audioformat.h"
#include "multistream.h"

namespace streamapp
{

    /**
      * MultiOutputWaveFile
      *   write a number mono wave files.
      *   Call mp_bOpen() to create ac_nChannels mono wavefiles,
      *   each will receive the corresponding channel of the Stream
      *   written.
      *   @see WaveFile
      *   @see MultiInputWaveFile
      *   @see MultiOutputStream
      ************************************************************* */
  class MultiOutputWaveFile : public MultiOutputStream<>
  {
  public:
      /**
        * Constructor.
        */
    MultiOutputWaveFile();

      /**
        * Destructor.
        * Calls mp_Close().
        * @see mp_Close()
        */
    ~MultiOutputWaveFile();

      /**
        * Open the output files for writing.
        * Creates file if non-existant, else overwrites them.
        * Creates ac_nChannels files, named ac_sFileNamePrefix_chxx.wav,
        * where 'xx' stands for the channel number.
        * @param ac_nChannels the number of channels aka files
        * @param ac_nBufferSize the buffersize
        * @param ac_lSampleRate the samplerate
        * @param ac_sFileNamePrefix the filename prefix string, can include full path
        * @param ac_eBitMode one of AudioFormat::mt_eBitMode
        * @return false if there was an error opening the file(s)
        */
    bool mp_bOpen( const unsigned ac_nChannels, const unsigned ac_nBufferSize, const unsigned long ac_lSampleRate,
                   const std::string& ac_sFileNamePrefix, const AudioFormat::mt_eBitMode ac_eBitMode = AudioFormat::MSBint16 );

      /**
        * Close the files.
        * Doesn't do anything if files aren't open.
        */
    void mp_Close();
  };

  /**
    * MultiInputWaveFile
    *   counterpart of the MultiOutputWaveFile, reads a number of mono wave files.
    *   The wavefiles are combined into one PositionableInputStream with as much
    *   channels as files were found. Expects ac_sFileNamePrefix_ch(0..n).wav
    *   filenames, and all files must be equal in length, samplerate and bitmode.
    *   @see InputWaveFile
    *   @see MultiOutputWaveFile
    *   @see MultiPosInputStream
    ****************************************************************************** */
  class MultiInputWaveFile : public MultiPosInputStream<>
  {
  public:
      /**
        * Constructor.
        */
    MultiInputWaveFile();

      /**
        * Destructor.
        * Calls mp_Close().
        * @see mp_Close()
        */
    ~MultiInputWaveFile();

      /**
        * Open files for reading.
        * The number of channels will be the number of files found with the
        * ac_sFileNamePrefix_chxx.wav name, where xx stans for the channelnumber.
        * All files must be equal in length, and all files must have the same
        * samplerate and bitmode.
        * @param ac_nBufferSize the buffersize
        * @param ac_sFileNamePrefix the filename prefix string
        * @return true if all files are sucessfully opened
        */
    bool mp_bOpen( const unsigned ac_nBufferSize, const std::string& ac_sFileNamePrefix );

      /**
        * Close any open files.
        * Doesn't do anything if no files are open.
        */
    void mp_Close();
  };

}

#endif //#ifndef __MULTIWAVEFILE_H__
