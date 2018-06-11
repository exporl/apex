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

#include "../appcore/deleter.h"

#include "../utils/stringexception.h"
#include "../utils/stringutils.h"
#include "../utils/systemutils.h"

#include "binaryfile.h"

#include <QString>
#include <QtAlgorithms>
#include <QtGlobal>

using namespace streamapp;
using namespace appcore;

namespace
{
const QString fileExtension(".sbin");
const QString channelPrefix("_ch");
}

BinaryOutputFile::BinaryOutputFile()
{
}

BinaryOutputFile::~BinaryOutputFile()
{
    mp_bClose();
}

bool BinaryOutputFile::mp_bOpen(const QString &fileNamePrefix,
                                const unsigned ac_nChannels)
{
    if (!files.isEmpty())
        return false;

    for (unsigned i = 0; i < ac_nChannels; ++i) {
        const QString fileName =
            fileNamePrefix + channelPrefix + i + fileExtension;
        QScopedPointer<QFile> file(new QFile(fileName));
        file->open(QIODevice::WriteOnly);
        files.append(file.take());
    }

    return true;
}

unsigned BinaryOutputFile::mf_nChannels() const
{
    return files.size();
}

bool BinaryOutputFile::mp_bClose()
{
    qDeleteAll(files.begin(), files.end());
    files.clear();
    return true;
}

void BinaryOutputFile::Write(const Stream &ac_Data)
{
    if (files.isEmpty())
        return;

    const unsigned channelCount = mf_nChannels();
    Q_ASSERT(ac_Data.mf_nGetChannelCount() >= channelCount);

    const unsigned c_nSize = ac_Data.mf_nGetBufferSize() * sizeof(StreamType);

    for (unsigned i = 0; i < channelCount; ++i)
        files[i]->write((char *)ac_Data.mf_pGetArray()[i], c_nSize);
}

unsigned long BinaryOutputFile::Write(const void **a_pBuf,
                                      const unsigned ac_nSamples)
{
    if (files.isEmpty())
        return 0L;

    const unsigned channelCount = mf_nChannels();
    const unsigned samplesInBytes = ac_nSamples * sizeof(StreamType);

    for (unsigned i = 0; i < channelCount; ++i)
        files[i]->write((char *)(((StreamType **)a_pBuf)[i]), samplesInBytes);

    return ac_nSamples;
}

/***************************************************************************************************************************/

BinaryInputFile::BinaryInputFile() : mv_nSamples(0), mv_nReadOffset(0)
{
}

BinaryInputFile::~BinaryInputFile()
{
    mp_bClose();
}

bool BinaryInputFile::mp_bOpen(const QString &fileNamePrefix)
{
    if (!files.isEmpty())
        return false;

    // search files
    unsigned channelCount = 0;
    QString fileName =
        fileNamePrefix + channelPrefix + channelCount + fileExtension;
    while (QFile::exists(fileName)) {
        ++channelCount;
        fileName =
            fileNamePrefix + channelPrefix + channelCount + fileExtension;
    }

    if (!channelCount)
        return false;

    for (unsigned i = 0; i < channelCount; ++i) {
        const QString fileName =
            fileNamePrefix + channelPrefix + i + fileExtension;
        QScopedPointer<QFile> file(new QFile(fileName));
        file->open(QIODevice::ReadOnly);
        if (i == 0) {
            mv_nSamples = file->size() / sc_nSampleSize;
        } else {
            // check same size
            if (((unsigned long)((file->size()) / sc_nSampleSize)) !=
                mv_nSamples) {
                // mismatch; clean up and fail
                qDeleteAll(files.begin(), files.end());
                files.clear();
                return false;
            }
        }

        files.append(file.take());
    }

    mv_nReadOffset = 0;

    return true;
}

bool BinaryInputFile::mp_bOpenAny(const QString &fileName)
{
    if (!files.isEmpty())
        return false;

    if (!QFile::exists(fileName))
        return false;

    QScopedPointer<QFile> file(new QFile(fileName));
    file->open(QIODevice::ReadOnly);
    files.append(file.take());

    mv_nSamples = file->size() / sc_nSampleSize;
    mv_nReadOffset = 0;

    return true;
}

bool BinaryInputFile::mp_bClose()
{
    qDeleteAll(files.begin(), files.end());
    files.clear();
    mv_nSamples = 0;
    return true;
}

unsigned long BinaryInputFile::Read(void **a_pBuf, const unsigned ac_nSamples)
{
    if (files.isEmpty())
        return 0L;
    if (mv_nReadOffset >= mv_nSamples)
        return 0L;

    StreamType **buffer = (StreamType **)a_pBuf;

    const unsigned channelCount = mf_nChannels();
    qint64 bytesRead = 0;

    for (unsigned i = 0; i < channelCount; ++i) {
        bytesRead =
            files[i]->read((char *)buffer[i], ac_nSamples * sc_nSampleSize);
    }

    mv_nReadOffset += bytesRead;

    return bytesRead;
}

unsigned BinaryInputFile::mf_nChannels() const
{
    return files.size();
}

unsigned long BinaryInputFile::mf_lSamplesLeft() const
{
    return mv_nSamples - mv_nReadOffset;
}

unsigned long BinaryInputFile::mf_lTotalSamples() const
{
    return mv_nSamples;
}

unsigned long BinaryInputFile::mf_lCurrentPosition() const
{
    return mv_nReadOffset;
}

void BinaryInputFile::mp_SeekPosition(const unsigned long ac_nPosition)
{
    if (ac_nPosition >= mf_lTotalSamples())
        throw utils::StringException("mp_SeekPosition got out of range!!");

    // convert nSamples to bytes
    const unsigned long bytesToAdvance = ac_nPosition * sc_nSampleSize;

    const unsigned channelCount = mf_nChannels();
    for (unsigned i = 0; i < channelCount; ++i) {
        files[i]->seek(bytesToAdvance);
    }
    mv_nReadOffset = ac_nPosition;
}
