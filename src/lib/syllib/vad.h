/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
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

#ifndef _SYL_SRC_LIB_VAD_H_
#define _SYL_SRC_LIB_VAD_H_

/** @file
 * Voice activity detector declaration.
 */

#include "global.h"

#include <QSharedDataPointer>

namespace syl
{

class VoiceActivityDetectorPrivate;

/** Voice activity detector. Was developed by Simon Doclo and Erik De Clippel.
 */
class SYL_EXPORT VoiceActivityDetector
{
public:
    /** Creates a new Voice activity detector.
     *
     * @param blockLength number of frames that is analysed at once
     * @param synchronous @c true if the VAD should operate synchronously,
     * i.e., if the frame number to be processed is always an integer multiple
     * of the block length; otherwise a delay of @a length frames is introduced
     */
    VoiceActivityDetector (unsigned blockLength, bool synchronous);

    /** Deletes all allocated data and frees the instance. */
    ~VoiceActivityDetector();

    VoiceActivityDetector (const VoiceActivityDetector &other);
    VoiceActivityDetector &operator= (const VoiceActivityDetector &other);

    /** Sets whether a special way of calculating the RMS value of a data block
     * should be used. This should normally not be necessary unless you want to
     * match the behaviour of legacy code. If this is @c true, RMS values
     * between @c 0.0 and @c 0.01 can give wrong results.
     *
     * @param value @c true if you want the weird way, @c false otherwise
     */
    void setWeirdRMSLimiting (bool value);

    /** Returns whether a special way of calculating the RMS value of a data block
     * is used.
     *
     * @return @c true if the weird way is used, @c false otherwise
     *
     * @see #setWeirdRMSLimiting()
     */
    bool weirdRMSLimiting() const;

    /** Returns the block length that is used by the VAD. If @a synchronous
     * was specified @c true at construction, the length of the data passed to
     * #process() must be an integer multiple of this value.
     *
     * @return block length
     */
    unsigned length() const;

    /** Returns the current stream position.
     *
     * @return stream position in frames
     */
    unsigned position() const;

    /** Processes a certain amount of data. If @a synchronous was specified @c
     * true at construction, the length of the data must be an integer multiple
     * of #length(). The result contains @c 1.0 for detected speech, @c 0.0 for
     * detected noise or @c 0.5 if it is not possible to determine this.
     *
     * @param data data to be analyzed, will also contain the output data
     * @param dataLength number of frames to analyze, if @a synchronous was
     * specified @c true at construction, this must be an integer multiple of
     * #length()
     */
    void process (double *data, unsigned dataLength);

private:
    QSharedDataPointer<VoiceActivityDetectorPrivate> d;
};

} // namespace syl

#endif
