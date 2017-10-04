/******************************************************************************
 * Copyright (C) 2008 Michael Hofmann <mh21@piware.de>                        *
 * Copyright (C) 2008 Division of Experimental Otorhinolaryngology K.U.Leuven *
 *                                                                            *
 * Original version written by Maarten Lambert.                               *
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

#ifndef _BERTHA_SRC_LIB_BERTHA_RINGBUFFER_H_
#define _BERTHA_SRC_LIB_BERTHA_RINGBUFFER_H_

#include "global.h"

#include <QAtomicInt>
#include <QVector>

namespace bertha
{

// Class is safe for write access from one and read access from another thread
template <typename T>
class RingBuffer
{
public:
    RingBuffer(unsigned bufferSize = 1000)
        : buffer(bufferSize), readPtr(0), writePtr(0)
    {
    }

    bool push(const T &data)
    {
        if (isFull())
            return false;

        buffer[writePtr] = data;
        writePtr.fetchAndStoreRelease((writePtr.load() + 1) % buffer.size());
        return true;
    }

    bool pushList(const QVector<T> &data)
    {
        for (int i = 0; i < data.size(); ++i) {
            if (isFullAtPos(writePtr.load() + i))
                return false;
            buffer[(writePtr.load() + i) % buffer.size()] = data[i];
        }

        writePtr.fetchAndStoreRelease((writePtr.load() + data.size()) %
                                      buffer.size());
        return true;
    }

    T pop()
    {
        if (isEmpty())
            return T();

        T result = buffer[readPtr.load()];
        readPtr.fetchAndStoreRelease((readPtr.load() + 1) % buffer.size());
        return result;
    }

    bool isEmpty() const
    {
        return writePtr.load() == readPtr.load();
    }

    bool isFull() const
    {
        return (writePtr.load() + 1) % buffer.size() == readPtr.load();
    }

    bool isFullAtPos(int position) const
    {
        return (position + 1) % buffer.size() == readPtr.load();
    }

private:
    QVector<T> buffer;
    QAtomicInt readPtr;
    QAtomicInt writePtr;
};

} // namespace bertha

#endif
