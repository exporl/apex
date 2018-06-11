/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "../_fromv3/core/containers/looplogic.h"

#include "../defines.h"

#include "../file/wavefile.h"

#include "../utils/dataconversion.h"
#include "../utils/stringexception.h"
#include "../utils/stringutils.h"
#include "../utils/tracer.h"

#include <QString>

Q_LOGGING_CATEGORY(wavefile, "apex.wavefile")

using namespace std;
using namespace dataconversion;

using namespace streamapp;

InputWaveFile::InputWaveFile()
    : totalSampleCount(0l),
      loopLogic(new mt_LoopLogic(totalSampleCount, mv_lLoopStart, mv_lLoopEnd,
                                 readOffset))
{
}

InputWaveFile::~InputWaveFile()
{
    mp_bClose();
    delete loopLogic;
}

bool InputWaveFile::mp_bOpen(const QString &fileName)
{
    // do not allow open more than once
    if (inputFile.isNull()) {
        inputFile.reset(new QFile(fileName));
        if (!inputFile->open(QIODevice::ReadOnly)) {
            inputFile.reset(0);
            return false;
        }
        ReadRIFF();
        ReadFMT();
        ReadDATA();

        dataLeft = dataHeader->dataSIZE;

        const short wFormat = fmtHeader->fmtFORMAT.wFormatTag;
        const short wBitsPS = fmtHeader->fmtFORMAT.wBitsPerSample;

        if (wBitsPS == 16 && wFormat == 1) {
            m_eBitMode = AudioFormat::MSBint16;
            bytesPerSample = 2;
        } else if (wBitsPS == 32 && wFormat == 1) {
            m_eBitMode = AudioFormat::MSBint32;
            bytesPerSample = 4;
        } else if (wBitsPS == 32 && wFormat == 3) {
            m_eBitMode = AudioFormat::MSBfloat32;
            bytesPerSample = 4;
        } else if (wBitsPS == 24 && wFormat == 1) {
            m_eBitMode = AudioFormat::MSBint24;
            bytesPerSample = 3;
        } else {
            throw(utils::StringException("unknown wave format"));
        }
        bytesPerFrame = mf_nChannels() * bytesPerSample;
        totalSampleCount = dataHeader->dataSIZE / bytesPerFrame;

        // some wave editors append extra bytes after the samples,
        // we have to check this here since we count up instead of down
        // new: just force correct file format instead of checking it every time
        qint64 cur, begin, end;
        cur = inputFile->pos();
        inputFile->seek(0);
        begin = inputFile->pos();
        inputFile->seek(inputFile->size());
        end = inputFile->pos();
        inputFile->seek(cur);

        long len = end - begin;
        long dsize = len - WaveFileData::waveHeaderSize;
        if ((unsigned long)dsize > totalSampleCount * bytesPerFrame) {
            qCDebug(wavefile) << "InputWaveFile: file has "
                              << dsize - totalSampleCount * bytesPerFrame
                              << " bytes too many";
        }

        return true;
    }
    return false;
}

bool InputWaveFile::mp_bClose()
{
    if (!inputFile.isNull() && inputFile->isOpen()) {
        inputFile.reset(0);
        return true;
    }
    return false;
}

unsigned long InputWaveFile::Read(void **a_pBuf, const unsigned ac_nSamples)
{
    if (!inputFile->isOpen()) {
        qCWarning(wavefile, "Attempt to read from closed wave file");
        throw(0);
    }

    const bool bLoop = mf_bIsLooping();
    if (!bLoop && readOffset >= totalSampleCount)
        return 0l; // eof

    unsigned nSamples = ac_nSamples;
    unsigned nRead = 0;

    int *dest0 = ((int **)a_pBuf)[0];
    int *dest1 = mf_nChannels() < 2 ? NULL : ((int **)a_pBuf)[1];

    while (nSamples > 0) {
        const unsigned nThisTime = s_min(tempBufSize / bytesPerFrame, nSamples);
        unsigned nBytesToRead = nThisTime * bytesPerFrame;
        unsigned nBytesRead = 0;
        unsigned nSamplesRead = 0;

        if (!bLoop) {
            nBytesRead = inputFile->read(tempBuffer,
                                         qMin(mf_lSamplesLeft() * bytesPerFrame,
                                              (unsigned long)nBytesToRead));
            nSamplesRead = nBytesRead / bytesPerFrame;
            readOffset += nSamplesRead;
        } else {
            unsigned nLeft = nThisTime - nSamplesRead;
            while (nLeft) {
                unsigned nLoopRead = loopLogic->mf_nGetPiece(nLeft);

                nBytesRead =
                    inputFile->read(tempBuffer + (nSamplesRead * bytesPerFrame),
                                    qMin(mf_lSamplesLeft() * bytesPerFrame,
                                         nLoopRead * bytesPerFrame));

                unsigned nReadLoop = nBytesRead / bytesPerFrame;
                nSamplesRead += nReadLoop;
                nLeft = nThisTime - nSamplesRead;

                mp_SeekPosition(loopLogic->mf_nGetNewPos(nReadLoop));
            }
        }

        nRead += nSamplesRead;

        // convert all nSamplesRead
        mf_Convert(dest0, dest1, nSamplesRead);

        // no need to try further
        if (nSamplesRead != nThisTime) {
            return nRead;
        }

        nSamples -= nThisTime;
    }
    return nRead;
}

void InputWaveFile::mf_Convert(int *&dest0, int *&dest1,
                               const unsigned nSamplesRead)
{
    const unsigned channelCount = mf_nChannels();

    if (m_eBitMode == AudioFormat::MSBint16) {
        const short *src = (const short *)tempBuffer;

        if (channelCount == 1) {
            for (unsigned i = 0; i < nSamplesRead; ++i)
                *dest0++ = *src++ << 16;
        } else {
            for (unsigned i = 0; i < nSamplesRead; ++i) {
                *dest0++ = *src++ << 16;
                *dest1++ = *src++ << 16;
            }
        }
    } else if (m_eBitMode == AudioFormat::MSBint24) {
        const char *src = (const char *)tempBuffer;
        if (channelCount == 1) {
            for (unsigned i = 0; i < nSamplesRead; ++i) {
                *dest0++ = littleEndian24Bit(src) << 8;
                src += 3;
            }
        } else {
            for (unsigned i = 0; i < nSamplesRead; ++i) {
                *dest0++ = littleEndian24Bit(src) << 8;
                src += 3;
                *dest1++ = littleEndian24Bit(src) << 8;
                src += 3;
            }
        }
    } else if (m_eBitMode == AudioFormat::MSBint32) {
        const int *src = (const int *)tempBuffer;
        if (channelCount == 1) {
            for (unsigned i = 0; i < nSamplesRead; ++i)
                *dest0++ = *src++;
        } else {
            for (unsigned i = 0; i < nSamplesRead; ++i) {
                *dest0++ = *src++;
                *dest1++ = *src++;
            }
        }
    } else if (m_eBitMode == AudioFormat::MSBfloat32) {
        const int *src = (const int *)tempBuffer;
        if (channelCount == 1) {
            for (unsigned i = 0; i < nSamplesRead; ++i)
                *dest0++ = *src++;
        } else {
            for (unsigned i = 0; i < nSamplesRead; ++i) {
                *dest0++ = *src++;
                *dest1++ = *src++;
            }
        }
    }
}

unsigned long InputWaveFile::mf_lSamplesLeft() const
{
    return totalSampleCount - readOffset;
}

unsigned long InputWaveFile::mf_lTotalSamples() const
{
    return totalSampleCount;
}

unsigned long InputWaveFile::mf_lCurrentPosition() const
{
    return readOffset;
}

void InputWaveFile::mp_SeekPosition(const unsigned long ac_nPosition)
{
    if (ac_nPosition >= mf_lTotalSamples())
        throw utils::StringException("mp_SeekPosition got out of range!!");
    // convert nSamples to bytes
    const unsigned long nToAdvance = ac_nPosition * bytesPerFrame;
    // seek pos, don't forget to jump over header
    inputFile->seek(nToAdvance + WaveFileData::waveHeaderSize);
    // update
    readOffset = ac_nPosition;
}
void InputWaveFile::ReadRIFF()
{
    inputFile->read((char *)riffHeader, sizeof(RIFF));
    if (!CheckID(riffHeader->riffID, 'R', 'I', 'F', 'F')) {
        throw(0);
    }
    if (!CheckID(riffHeader->riffFORMAT, 'W', 'A', 'V', 'E')) {
        throw(0);
    }
}
void InputWaveFile::ReadFMT()
{
    inputFile->read((char *)fmtHeader, sizeof(FMT));
    if (!CheckID(fmtHeader->fmtID, 'f', 'm', 't', ' ')) {
        throw(0);
    }
}
void InputWaveFile::ReadDATA()
{
    try {
        inputFile->read((char *)dataHeader, sizeof(DATA));
        if (!CheckID(dataHeader->dataID, 'd', 'a', 't', 'a'))
            throw(0);
    } catch (...) {
        // something stange! In wave files the DATA identifier
        // can be offseted (maybe because of address aligment)
        // Start to looking DATA_ID "manualy" ;)
        inputFile->seek(sizeof(RIFF) + sizeof(FMT));
        bool foundData = false;
        char ch;

        while (inputFile) {
            inputFile->getChar(&ch);
            if (ch == 'd') {
                // It can be DATA_ID, check it!
                inputFile->ungetChar(ch);
                inputFile->read((char *)dataHeader, sizeof(DATA));
                if (CheckID(dataHeader->dataID, 'd', 'a', 't', 'a')) {
                    foundData = true;
                    break;
                }
            }
        }

        if (!foundData)
            throw(0);
    }
}

bool InputWaveFile::CheckID(char *idPar, char A, char B, char C, char D)
{
    return ((idPar[0] == A) && (idPar[1] == B) && (idPar[2] == C) &&
            (idPar[3] == D));
}

/************************************************************* OutputWaveFile
 * *********************************************************/

OutputWaveFile::OutputWaveFile() : outputFile(0)
{
}

OutputWaveFile::~OutputWaveFile()
{
    mp_bClose();
}

bool OutputWaveFile::mp_bOpen(const QString &fileName,
                              const unsigned channelCount,
                              const unsigned long ac_nFs,
                              const AudioFormat::mt_eBitMode ac_eMode)
{
    // do not allow create more than once
    if (outputFile.isNull()) {
        if (channelCount == 0 || channelCount > 2 || ac_nFs == 0)
            return false;

        outputFile.reset(new QFile(fileName));
        if (!outputFile->open(QIODevice::WriteOnly | QIODevice::Truncate))
            return false;

        const short c_channelCount = (short)channelCount;

        m_eBitMode = ac_eMode;
        short wBitsPS = 0;
        short wFormat = 0;
        short wBlockA = 0;
        if (m_eBitMode == AudioFormat::MSBint16) {
            wBitsPS = 16;
            wFormat = 1;
            wBlockA = 2 * c_channelCount;
            bytesPerSample = 2;
        } else if (m_eBitMode == AudioFormat::MSBint32) {
            wBitsPS = 32;
            wFormat = 1;
            wBlockA = 4 * c_channelCount;
            bytesPerSample = 4;
        } else if (m_eBitMode == AudioFormat::MSBfloat32) {
            wBitsPS = 32;
            wFormat = 3;
            wBlockA = 4 * c_channelCount;
            bytesPerSample = 4;
        } else if (m_eBitMode == AudioFormat::MSBint24) {
            wBitsPS = 24;
            wFormat = 1;
            wBlockA = 3 * c_channelCount;
            bytesPerSample = 3;
        } else {
            throw(utils::StringException("unknown wave format"));
        }

        bytesPerFrame = channelCount * bytesPerSample;

        // RIFF
        strncpy(riffHeader->riffID, "RIFF", 4);
        riffHeader->riffSIZE = waveHeaderSize - 8;
        strncpy(riffHeader->riffFORMAT, "WAVE", 4);

        // FMT
        strncpy(fmtHeader->fmtID, "fmt ", 4);
        fmtHeader->fmtSIZE = sizeof(WAVEFORM);
        fmtHeader->fmtFORMAT.wFormatTag = wFormat;
        fmtHeader->fmtFORMAT.channelCount = (short)channelCount;
        fmtHeader->fmtFORMAT.samplesPerSecond = ac_nFs;
        fmtHeader->fmtFORMAT.nBlockAlign = wBlockA;
        fmtHeader->fmtFORMAT.nAvgBytesPerSec = bytesPerFrame * ac_nFs;
        fmtHeader->fmtFORMAT.wBitsPerSample = wBitsPS;

        // DATA
        strncpy(dataHeader->dataID, "data", 4);
        dataHeader->dataSIZE = 0;

        outputFile->write((char *)riffHeader, sizeof(RIFF));
        outputFile->write((char *)fmtHeader, sizeof(FMT));
        outputFile->write((char *)dataHeader, sizeof(DATA));

        return true;
    }

    return false;
}

bool OutputWaveFile::mp_bClose()
{
    if (!outputFile.isNull()) {
        WriteHeader();
        outputFile.reset(0);
        return true;
    }
    return false;
}

void OutputWaveFile::WriteHeader()
{
    outputFile->seek(0);
    riffHeader->riffSIZE += dataLeft;
    dataHeader->dataSIZE += dataLeft;
    outputFile->write((char *)riffHeader, sizeof(RIFF));
    outputFile->write((char *)fmtHeader, sizeof(FMT));
    outputFile->write((char *)dataHeader, sizeof(DATA));
}

unsigned long OutputWaveFile::Write(const void **a_pBuf,
                                    const unsigned ac_nSamples)
{
    if (!outputFile->isOpen())
        throw(utils::StringException(
            "OutputWaveFile::Write")); // should *never* come here

    unsigned nSamples = ac_nSamples;
    const unsigned channelCount = mf_nChannels();

    const int *src0 = (const int *)a_pBuf[0];
    const int *src1 = (const int *)a_pBuf[1];

    while (nSamples > 0) {
        const unsigned nThisTime = s_min(tempBufSize / bytesPerFrame, nSamples);
        const unsigned nToWrite = nThisTime * bytesPerFrame;

        if (m_eBitMode == AudioFormat::MSBint16) {
            short *dest = (short *)tempBuffer;
            if (channelCount == 1) {
                for (unsigned i = 0; i < nThisTime; ++i)
                    *dest++ = (short)(*src0++ >> 16);
            } else {
                for (unsigned i = 0; i < nThisTime; ++i) {
                    *dest++ = (short)(*src0++ >> 16);
                    *dest++ = (short)(*src1++ >> 16);
                }
            }
        } else if (m_eBitMode == AudioFormat::MSBint24) {
            char *dest = (char *)tempBuffer;
            if (channelCount == 1) {
                for (unsigned i = 0; i < nThisTime; ++i) {
                    littleEndian24BitToChars(*src0++ >> 8, dest);
                    dest += 3;
                }
            } else {
                for (unsigned i = 0; i < nThisTime; ++i) {
                    littleEndian24BitToChars(*src0++ >> 8, dest);
                    dest += 3;
                    littleEndian24BitToChars(*src1++ >> 8, dest);
                    dest += 3;
                }
            }
        } else if (m_eBitMode == AudioFormat::MSBint32) {
            int *dest = (int *)tempBuffer;
            if (channelCount == 1) {
                for (unsigned i = 0; i < nThisTime; ++i)
                    *dest++ = *src0++;
            } else {
                for (unsigned i = 0; i < nThisTime; ++i) {
                    *dest++ = *src0++;
                    *dest++ = *src1++;
                }
            }
        } else if (m_eBitMode == AudioFormat::MSBfloat32) {
            int *dest = (int *)tempBuffer;
            if (channelCount == 1) {
                for (unsigned i = 0; i < nThisTime; ++i)
                    *dest++ = *src0++;
            } else {
                for (unsigned i = 0; i < nThisTime; ++i) {
                    *dest++ = *src0++;
                    *dest++ = *src1++;
                }
            }
        }

        if (outputFile->write(tempBuffer, nToWrite) == -1) {
            // disk full or so, write header so we still have usable data
            WriteHeader();
            return 0l;
        }

        dataLeft += nToWrite;
        nSamples -= nThisTime;
    }

    return ac_nSamples;
}
