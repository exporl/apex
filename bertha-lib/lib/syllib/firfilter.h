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

#ifndef _BERTHA_SRC_LIB_SYLLIB_FIRFILTER_H_
#define _BERTHA_SRC_LIB_SYLLIB_FIRFILTER_H_

/** @file
 * FIR filter declaration.
 */

#include "global.h"

#include <QSharedDataPointer>
#include <QString>
#include <QVector>

class QIODevice;

namespace syl
{

class FirFilterPrivate;

/** FIR filter implemented in the frequency domain. The filter is implicitly
 * shared (copy-on-write) and cheap to copy. Filters are limited to 16k channels
 * and 64k taps.
 *
 * The filter coefficients can be read from a file with the following format:
 *
 * @code
 * size  type    description
 *    8  double  number of channels (c)
 *    8  double  number of filter coefficients per channel (n)
 *  n*8  double  filter coefficients of the first channel
 *  n*8  double  filter coefficients of the second channel
 *    .
 *    .
 *    .
 * @endcode
 */
class SYL_EXPORT FirFilter
{
public:
    /** Creates a new FIR filter. The number of filter taps is extended to the
     * next power of 2.
     *
     * @param taps filter taps
     * @param synchronous @c true if the filter should operate synchronously,
     * i.e., if the frame number to be processed is always an integer multiple
     * of the filter length; otherwise a delay of @a length frames is introduced
     */
    FirFilter(const QVector<double> &taps, bool synchronous);

    /** Deletes all allocated data and frees the instance. */
    virtual ~FirFilter();

    FirFilter(const FirFilter &other);
    FirFilter &operator=(const FirFilter &other);

    /** Loads filter coefficients from a file.
     *
     * @param filePath full file path
     * @param maximumLength maximum number of taps for the filter (0 means
     * unlimited), additional taps will be ignored
     * @return filter coefficients sorted by channel
     */
    static QList<QVector<double>> load(const QString &filePath,
                                       unsigned maximumLength = 0);

    /** Loads filter coefficients from an QIODevice.
     *
     * @param device QIODevice to read from
     * @param maximumLength maximum number of taps for the filter (0 means
     * unlimited), additional taps will be ignored
     * @return filter coefficients sorted by channel
     */
    static QList<QVector<double>> load(QIODevice *device,
                                       unsigned maximumLength = 0);

    /** Returns the filter length that is used by the filter. If @a synchronous
     * was specified @c true at construction, the length of the data passed to
     * #process() must be an integer multiple of this value.
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

    /** Processes a certain amount of data. If @a synchronous was specified @c
     * true at construction, the length of the data must be an integer multiple
     * of #length().
     *
     * @param data data to be filtered, will also contain the output data
     * @param dataLength number of frames to filter, if @a synchronous was
     * specified @c true at construction, this must be an integer multiple of
     * #length()
     */
    void process(double *data, unsigned dataLength);

private:
    QSharedDataPointer<FirFilterPrivate> d;
};

} // namespace syl

#endif
