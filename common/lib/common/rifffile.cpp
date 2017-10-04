/******************************************************************************
 * Copyright (C) 2007-2015  Michael Hofmann <mh21@mh21.de>                    *
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

#include "rifffile.h"
#include "exception.h"

#include <QIODevice>

#include <QtEndian>

namespace cmn
{

// RiffFile ====================================================================

RiffFile::RiffFile(QIODevice *device, bool ownDevice)
    : file(device), ownedFile(ownDevice ? device : NULL)
{
}

quint32 RiffFile::makeId(const char *name)
{
    return qFromLittleEndian<quint32>(reinterpret_cast<const uchar *>(name));
}

quint32 RiffFile::readChunkHeader()
{
    uchar chunkHeader[8];
    if (file->read(reinterpret_cast<char *>(chunkHeader), 8) != 8)
        throw Exception(tr("Unable to read chunk header"));
    const quint32 result = qFromLittleEndian<quint32>(chunkHeader);
    nextLength = qFromLittleEndian<quint32>(chunkHeader + 4);
    nextStart = file->pos();
    return result;
}

void RiffFile::skipChunk()
{
    if (!file->seek(nextStart + (nextLength + 1) / 2 * 2))
        throw Exception(tr("Unable to seek to end of the current chunk"));
}

void RiffFile::skipToChunk(quint32 id)
{
    Q_FOREVER {
        if (file->atEnd())
            throw Exception(tr("chunk not found"));
        if (readChunkHeader() == id)
            break;
        skipChunk();
    }
}

void RiffFile::writeChunkHeader(quint32 id, quint32 length)
{
    uchar chunkHeader[8];
    qToLittleEndian<quint32>(id, chunkHeader);
    qToLittleEndian<quint32>(length, chunkHeader + 4);
    if (file->write(reinterpret_cast<char *>(chunkHeader), 8) != 8)
        throw Exception(tr("Unable to write Chunk header"));
}

} // namespace cmn
