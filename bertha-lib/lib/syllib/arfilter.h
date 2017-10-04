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

#ifndef _BERTHA_SRC_LIB_SYLLIB_ARFILTER_H_
#define _BERTHA_SRC_LIB_SYLLIB_ARFILTER_H_

/** @file
 * Auto-regressive filter declaration.
 */

#include "global.h"

#include <QSharedDataPointer>
#include <QVector>

class QIODevice;

namespace syl
{

class ArFilterPrivate;

/** Auto-regressive filter. The filter is implicitly shared (copy-on-write) and
 * cheap to copy. Filters are limited to 64k taps.
 *
 * The filter coefficients can be read from a file with the following format:
 *
 * @code
 * size  type    description
 *    8  double  number of channels (c), always 2
 *    8  double  number of filter coefficients per channel (n)
 *  n*8  double  MA filter coefficients, must be 1.0 followed zeros
 *  n*8  double  actual filter coefficients, first coefficient must be 1.0
 * @endcode
 */
class SYL_EXPORT ArFilter
{
public:
    /** Creates a new AR filter. The filter has no delay.
     *
     * @param taps filter taps, the first filter coefficient must be 1.0
     */
    explicit ArFilter(const QVector<double> &taps);

    /** Deletes all allocated data and frees the instance. */
    virtual ~ArFilter();

    ArFilter(const ArFilter &other);
    ArFilter &operator=(const ArFilter &other);

    /** Loads filter coefficients from a file.
     *
     * @param filePath full file path
     * @param maximumLength maximum number of taps for the filter (0 means
     * unlimited), additional taps will be ignored
     * @return filter coefficients
     */
    static QVector<double> load(const QString &filePath,
                                unsigned maximumLength = 0);

    /** Loads filter coefficients from an QIODevice.
     *
     * @param device QIODevice to read from
     * @param maximumLength maximum number of taps for the filter (0 means
     * unlimited), additional taps will be ignored
     * @return filter coefficients
     */
    static QVector<double> load(QIODevice *device, unsigned maximumLength = 0);

    /** Returns the filter length that is used by the filter.
     *
     * @return filter length
     */
    unsigned length() const;

    /** Returns the filter taps.
     *
     * @return filter taps
     */
    QVector<double> taps() const;

    /** Returns the current stream position.
     *
     * @return stream position in frames
     */
    unsigned position() const;

    /** Processes a certain amount of data.
     *
     * @param data data to be filtered, will also contain the output data
     * @param dataLength number of frames to filter
     */
    void process(double *data, unsigned dataLength);

private:
    QSharedDataPointer<ArFilterPrivate> d;
};

} // namespace syl

#endif
