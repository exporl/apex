/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
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

#include "audioconversion.h"

#include <QtCore>

namespace bertha
{

class AudioConversionPrivate
{
public:
    AudioConversionPrivate(AudioConversion::SampleFormat format)
        : format(format),
          width(format == AudioConversion::Int32SampleFormat
                    ? 4
                    : format == AudioConversion::Int16SampleFormat
                          ? 2
                          : format == AudioConversion::FloatSampleFormat ? 4
                                                                         : 8)
    {
        Q_ASSERT(format == AudioConversion::Int32SampleFormat ||
                 format == AudioConversion::Int16SampleFormat ||
                 format == AudioConversion::FloatSampleFormat);
    }

    const AudioConversion::SampleFormat format;
    const unsigned width;
};

// AudioConversion =============================================================

AudioConversion::AudioConversion(SampleFormat format)
    : d(new AudioConversionPrivate(format))
{
}

AudioConversion::~AudioConversion()
{
    delete d;
}

unsigned AudioConversion::sampleWidth() const
{
    return d->width;
}

void AudioConversion::from(const void *legacy, float *data, unsigned number)
{
    const unsigned width = d->width;
    for (unsigned i = 0; i < number; ++i) {
        if (d->format == Int32SampleFormat)
            data[i] = qFromLittleEndian<qint32>(
                          reinterpret_cast<const uchar *>(legacy) + i * width) /
                      2147483647.0;
        else if (d->format == Int16SampleFormat)
            data[i] = qFromLittleEndian<qint16>(
                          reinterpret_cast<const uchar *>(legacy) + i * width) /
                      32767.0;
        else
            data[i] = *(reinterpret_cast<const float *>(legacy) + i);
    }
}

void AudioConversion::to(const float *data, void *legacy, unsigned number)
{
    const unsigned width = d->width;
    for (unsigned i = 0; i < number; ++i) {
        if (d->format == Int32SampleFormat)
            qToLittleEndian<qint32>(qint32(qRound(data[i] * 2147483647.0)),
                                    reinterpret_cast<uchar *>(legacy) +
                                        i * width);
        else if (d->format == Int16SampleFormat)
            qToLittleEndian<qint16>(qint16(qRound(data[i] * 32767.0)),
                                    reinterpret_cast<uchar *>(legacy) +
                                        i * width);
        else
            *(reinterpret_cast<float *>(legacy) + i) = data[i];
    }
}

} // namespace bertha
