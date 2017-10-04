/******************************************************************************
 * Copyright (C) 2010 Michael Hofmann <mh21@piware.de>                        *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 3 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with this program; if not, write to the Free Software Foundation, Inc.,    *
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                *
 ******************************************************************************/

#include "sndfilewrapper.h"

#include "exception.h"

namespace cmn
{

SndFileData::SndFileData(SNDFILE *file) : file(file)
{
}

SndFileData::SndFileData(const QString &fileName, int mode, SF_INFO *sfInfo)
{
    qFile.setFileName(fileName);
    switch (mode) {
    case SFM_READ:
        qFile.open(QIODevice::ReadOnly);
        break;
    case SFM_WRITE:
        qFile.open(QIODevice::WriteOnly);
        break;
    case SFM_RDWR:
        qFile.open(QIODevice::ReadWrite);
        break;
    default:
        break;
    }
    if (!qFile.isOpen())
        throw Exception(tr("Unable to open file %1").arg(fileName));

    virtualIO.get_filelen = SndFileData::length;
    virtualIO.seek = SndFileData::seek;
    virtualIO.read = SndFileData::read;
    virtualIO.write = SndFileData::write;
    virtualIO.tell = SndFileData::tell;
    file = sf_open_virtual(&virtualIO, mode, sfInfo, (void *)this);
}

SndFileData::~SndFileData()
{
    close();
}

void SndFileData::close()
{
    if (file)
        sf_close(file);
    file = nullptr;
}

sf_count_t SndFileData::length(void *userData)
{
    return ((SndFileData *)userData)->qFile.size();
}

sf_count_t SndFileData::seek(sf_count_t offset, int whence, void *userData)
{
    QFile &qFile = ((SndFileData *)userData)->qFile;
    switch (whence) {
    case SEEK_SET:
        return qFile.seek(offset) ? qFile.pos() : -1;
    case SEEK_CUR:
        return qFile.seek(qFile.pos() + offset) ? qFile.pos() : -1;
    case SEEK_END:
        return qFile.seek(qFile.size() + offset) ? qFile.pos() : -1;
    default:
        return -1;
    }
}

sf_count_t SndFileData::read(void *buffer, sf_count_t count, void *userData)
{
    return ((SndFileData *)userData)->qFile.read((char *)buffer, count);
}

sf_count_t SndFileData::write(const void *buffer, sf_count_t count,
                              void *userData)
{
    return ((SndFileData *)userData)->qFile.write((char *)buffer, count);
}

sf_count_t SndFileData::tell(void *userData)
{
    return ((SndFileData *)userData)->qFile.pos();
}

// SndFile=======================================================================

SndFile::SndFile() : d(new SndFileData(nullptr))
{
}

SndFile::SndFile(SNDFILE *file) : d(new SndFileData(file))
{
}

SndFile::SndFile(const QString &fileName, int mode, SF_INFO *sfInfo)
    : d(new SndFileData(fileName, mode, sfInfo))
{
}

SndFile::~SndFile()
{
}

void SndFile::reset()
{
    d = new SndFileData(nullptr);
}

void SndFile::reset(SNDFILE *file)
{
    d->close();
    d = new SndFileData(file);
}

void SndFile::reset(const QString &fileName, int mode, SF_INFO *sfInfo)
{
    d->close();
    d = new SndFileData(fileName, mode, sfInfo);
}

bool SndFile::operator!() const
{
    return d->file == nullptr;
}

SndFile::operator bool() const
{
    return d->file;
}

SNDFILE *SndFile::get() const
{
    return d->file;
}

int SndFile::channels(const QString &fileName)
{
    SF_INFO sfInfo;
    SndFile(fileName, SFM_READ, &sfInfo);
    return sfInfo.channels;
}

sf_count_t SndFile::frames(const QString &fileName)
{
    SF_INFO sfInfo;
    SndFile(fileName, SFM_READ, &sfInfo);
    return sfInfo.frames;
}
}
