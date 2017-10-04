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

#ifndef _EXPORL_COMMON_LIB_COMMON_RIFFFILE_H_
#define _EXPORL_COMMON_LIB_COMMON_RIFFFILE_H_

#include "global.h"

#include <QCoreApplication>

class QIODevice;

namespace cmn
{

class COMMON_EXPORT RiffFile
{
    Q_DECLARE_TR_FUNCTIONS(RiffFile)
public:
    /** Open a riff file from the given QIODevice. This class takes ownership
     * of the device(if requested) and the waveformat.
     *
     * @param device device for riff file access
     */
    RiffFile(QIODevice *device, bool ownDevice);

    /** Reads the chunk header of the next chunk in the wave file. The chunk
     * length in bytes is stored in #nextLength.
     *
     * @return chunk id
     *
     * @throws std::exception if the chunk header could not be read
     */
    quint32 readChunkHeader();

    /** Skips the remainder of the current chunk.
     *
     * @throws std::exception if the seek could not be completed
     */
    void skipChunk();

    /** Skips all non-matching chunks in the search for the given chunk. Fails
     * with an exception or stops at the data part of the chunk.
     *
     * @param id chunk id
     * @param name chunk name for exception message
     *
     * @throws std::exception if the end of file was reached before the chunk
     * was found or an I/O error occurs
     */
    void skipToChunk(quint32 id);

    /** Writes the chunk header of the next chunk in the wave file.
     *
     * @param id chunk id
     * @param length chunk length in bytes
     *
     * @throws std:exception if the chunk header could not be written
     */
    void writeChunkHeader(quint32 id, quint32 length);

protected:
    /** Wave file descriptor. Contains the QIODevice that is used to read and
     * write the wave file.
     */
    QIODevice *const file;
    const QScopedPointer<QIODevice> ownedFile;

    /** Length of the current riff chunk.
    */
    quint32 nextLength;

    /** Start of the current riff chunk.
    */
    qint64 nextStart;

    static quint32 makeId(const char *name);
};

} // namespace cmn

#endif
