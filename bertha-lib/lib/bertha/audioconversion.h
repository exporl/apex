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

/** @file
 * Interfaces for audio data reading and writing.
 */

#ifndef _BERTHA_SRC_LIB_BERTHA_AUDIOCONVERSION_H_
#define _BERTHA_SRC_LIB_BERTHA_AUDIOCONVERSION_H_

#include "global.h"

namespace bertha
{

class AudioConversionPrivate;

/** Converts buffers with an encoding like int32 or int16 from and to float.
 * The maximum value of int16 buffers is 32767, of int32 buffers 2147483647.
 * These values are SHORT_MAX - 1 and INT_MAX - 1 to prevent generated
 * samples(in the range from -1.0 to 1.0 float) from clipping while saving
 * to an external wave file. This means that wavefiles with samples -32768
 * (int16) or -2147483648 (int32) will result in float samples below -1.0.
 * No check for clipping is done.
 * \todo Float values are not endianess-corrected
 */
class BERTHA_EXPORT AudioConversion
{
public:
    /** Sample format. The format decides over size and encoding of a sample. */
    enum SampleFormat {
        /// 32bit IEEE float.
        FloatSampleFormat,
        /// 16bit signed LSB integer.
        Int16SampleFormat,
        /// 32bit signed LSB integer.
        Int32SampleFormat,
        /// Unsupported sample format.
        UnknownSampleFormat
    };

    /** Creates a new sample type converter. The given SampleFormat determines
     * the legacy sample format that is used as source or target of the
     * conversion.
     *
     * @param format format of the samples
     */
    AudioConversion(SampleFormat format);

    /** Destroys the converter.
     */
    ~AudioConversion();

    /** Returns the sample width in bytes. Should be used to calculate the
     * amount of data that has to be read into the conversion buffer for a
     * certain number of samples.
     *
     * @return sample width in bytes
     */
    unsigned sampleWidth() const;

    /** Converts samples to float.
     *
     * @param legacy pointer to the buffer from where the samples should be read
     * @param data pointer to the buffer where the processed samples should be
     * stored
     * @param number number of samples to be processed
     */
    void from(const void *legacy, float *data, unsigned number);

    /** Converts samples from float.
     *
     * @param data pointer to the buffer from where the samples should be read
     * @param legacy pointer to the buffer where the processed samples should be
     * stored
     * @param number number of samples to be processed
     */
    void to(const float *data, void *legacy, unsigned number);

private:
    AudioConversionPrivate *const d;
};

} // namespace bertha

#endif
