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

#ifndef _SYL_SRC_LIB_ADAPTIVEWIENER_H_
#define _SYL_SRC_LIB_ADAPTIVEWIENER_H_

/** @file
 * Adaptive Wiener filter declaration.
 */

#include "global.h"

#include <memory>

namespace syl
{

class AdaptiveWienerFilterPrivate;

/** Adaptive Wiener filtering. For details see: S. Doclo and M. Moonen.
 * Gsvd-based optimal filtering for single and multi-microphone speech
 * enhancement. IEEE Transactions on Signal Processing, 50 (9):2230–2244, 2002.
 *
 * @todo
 * - seperate update of coefficients and filtering, reuse FirFilter
 * - better testbench for all the different parameters
 * - non-synchronous operation
 */
class SYL_EXPORT AdaptiveWienerFilter
{
public:
    /** Creates a new adaptive Wiener filter.
     *
     * @param sampleRate sample rate in Hz
     * @param channels number of input channels to filter
     * @param length filter length
     * @param muInv 1/μ parameter of the filter
     * @param synchronous @c true if the filter should operate synchronously,
     * i.e. if the frame number to be processed is always an integer multiple of
     * the filter length; otherwise a delay of @a length frames is introduced.
     * Only synchronous operation is implemented yet.
     */
    AdaptiveWienerFilter (unsigned sampleRate, unsigned channels,
            unsigned length, double muInv, bool synchronous);

    /** Deletes all allocated data and frees the instance. */
    virtual ~AdaptiveWienerFilter();

    /** Returns the current stream position.
     *
     * @return stream position in frames
     */
    unsigned position() const;

    /** Returns the filter length that is used by the filter. If @a synchronous
     * was specified @c true at construction, the length of the data passed to
     * #process() must be an integer multiple of this value.
     *
     * @return filter length
     */
    unsigned length() const;

    /** Processes a certain amount of data. If @a synchronous was specified @c
     * true at construction, the length of the data must be an integer multiple
     * of #length(). The data in @a dataSnr is used to update the filter
     * coefficients.
     *
     * @param data data to be filtered only, the first channel will contain the
     * output data
     * @param dataSnr data to be used for updating of the filter coefficients
     * @param vad voice activity data, only the first sample for each #length()
     * frames will be used, speech is denoted by @c 1.0 and noise by @c 0.0
     * @param dataLength number of frames to filter, if @a synchronous was
     * specified @c true at construction, this must be an integer multiple of
     * #length()
     */
    void process (double * const *data, const double * const *dataSnr,
            const double *vad, unsigned dataLength);

private:
    AdaptiveWienerFilter (const AdaptiveWienerFilter &other);

    const std::auto_ptr<AdaptiveWienerFilterPrivate> d;
};

} // namespace syl

#endif

