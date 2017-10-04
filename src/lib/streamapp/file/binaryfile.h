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

#ifndef __BINARYFILE_H__
#define __BINARYFILE_H__

#include "../audioformat.h"
#include "../stream.h"

#include <QFile>
#include <QList>
#include <QString>

namespace streamapp
{

/**
  * BinaryOutputFile
  *   AudioFormatWriter implementation with a binary file.
  *   The binary file just contains the 64-bit floating point
  *   samples, without any other information.
  *   Always writes one file per channel.
  *   Doesn't completely implement AudioFormat since
  *   the samplerate is not saved.
  *********************************************************** */
class BinaryOutputFile : public AudioFormatWriter
{
public:
    /**
      * Constructor.
      * Doesn't open any files, just sets up.
      */
    BinaryOutputFile();

    /**
      * Destructor.
        * Closes any open files.
        * @see mp_bClose()
      */
    ~BinaryOutputFile();

    /**
      * Open output file(s) for writing.
      * Filenames are fileNamePrefix + "_chxxx" with xxx the channel number.
      * @param fileNamePrefix the prefix (must include path)
      * @param ac_nChannels number of channels (==files) to write
      * @return false if the file(s) could not be opened
      */
    bool mp_bOpen(const QString &fileNamePrefix, const unsigned ac_nChannels);

    /**
      * Close the file(s).
      * @return false if file(s) could not be closed.
      */
    bool mp_bClose();

    /**
      * Implementation of the AudioFormatWriter method.
      */
    unsigned long Write(const void **a_pBuf, const unsigned ac_nSamples);

    /**
      * Implementation of the AudioFormat method.
      */
    unsigned mf_nChannels() const;

    /**
      * Implementation of the AudioFormat method.
      */
    unsigned long mf_lSampleRate() const
    {
        return 0L;
    }

    /**
      * Implementation of the AudioFormat method.
      */
    AudioFormat::mt_eBitMode mf_eBitMode() const
    {
        return AudioFormat::MSBfloat64;
    }

    /**
      * Write a stream.
      * Provided for convenience.
      * Writes all samples from mf_nGetNumChannels() channels of ac_Data.
      * Doesn't do anything if mp_bOpen isn't called.
      * @ac_Data the stream to write
      */
    void Write(const Stream &ac_Data);

private:
    QList<QFile *> files;
};

/**
  * BinaryInputFile
  *   PositionableAudioFormatReader with a binary file.
  *   Reads 64-bit floating point samples.
  *   Expects output from BinaryOutputFile, hence one
  *   file per channel.
  *   @see BinaryOutputFile
  **************************************************** */
class BinaryInputFile : public PositionableAudioFormatReader
{
public:
    /**
      * Constructor.
      */
    BinaryInputFile();

    /**
      * Destructor.
      */
    ~BinaryInputFile();

    /**
      * Open input file(s) for reading.
      * Filenames are fileNamePrefix + "_chxxx" with xxx the channel number.
      * The number of channels used will be the number of matching filenames
     * found.
      * @param fileNamePrefix the file prefix
      * @return false if the file could not be opened
      */
    bool mp_bOpen(const QString &fileNamePrefix);

    /**
      * Open a single input for writing.
      * The syntax of the filename doesn't matter, nor does the extension.
      * The number of channels used is one.
      * @param fileName the filename
      * @return false if the file could not be opened
      */
    bool mp_bOpenAny(const QString &fileName);

    /**
      * Close the file(s).
      * @return false if file(s) could not be closed.
      */
    bool mp_bClose();

    /**
      * Implementation of the AudioFormatReader method
      */
    unsigned long Read(void **a_pBuf, const unsigned ac_nSamples);

    /**
      * Implementation of the AudioFormat method
      */
    unsigned mf_nChannels() const;

    /**
      * Implementation of the AudioFormat method
      */
    unsigned long mf_lSampleRate() const
    {
        return 0L;
    }

    /**
      * Implementation of the AudioFormat method
      */
    AudioFormat::mt_eBitMode mf_eBitMode() const
    {
        return AudioFormat::MSBfloat64;
    }

    /**
      * Implementation of the PositionableAudioFormatReader method
      */
    unsigned long mf_lSamplesLeft() const;

    /**
      * Implementation of the PositionableAudioFormatReader method
      */
    unsigned long mf_lTotalSamples() const;

    /**
      * Implementation of the PositionableAudioFormatReader method
      */
    unsigned long mf_lCurrentPosition() const;

    /**
      * Implementation of the PositionableAudioFormatReader method
      */
    void mp_SeekPosition(const unsigned long ac_nPosition);

    /**
      * Size of one sample in bytes.
      */
    static const unsigned sc_nSampleSize = sizeof(StreamType);

private:
    QList<QFile *> files;
    unsigned long mv_nSamples;
    unsigned long mv_nReadOffset;
};
}

#endif //#ifndef __BINARYFILE_H__
