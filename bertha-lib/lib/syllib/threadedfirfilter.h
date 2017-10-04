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

#ifndef _BERTHA_SRC_LIB_SYLLIB_THREADEDFIRFILTER_H_
#define _BERTHA_SRC_LIB_SYLLIB_THREADEDFIRFILTER_H_

/** @file
 * Threaded FIR filter declaration.
 */

#include "firfilter.h"
#include "global.h"

#include <QObject>
#include <QString>
#include <QVector>

class QIODevice;
class QSemaphore;

namespace syl
{

/** FIR filter implementation for multi-threaded operation.
 * Allows the use of
 *
 * @code
 * // Setup
 * syl::EventThread eventThread;
 * eventThread.start();
 * syl::ThreadedFirFilter filter (taps, true);
 * filter.moveToThread (&eventThread);
 * qRegisterMetaType<double*>("double*");
 * qRegisterMetaType<QSemaphore*>("QSemaphore*");
 * QSemaphore semaphore;
 *
 * // Start calculation
 * QMetaObject::invokeMethod (&filter, "process",
 *     Q_ARG (double*, data),
 *     Q_ARG (unsigned int, blockSize),
 *     Q_ARG (QSemaphore*, &semaphore));
 *
 * // wait for completion
 * semaphore.acquire (1);
 * @endcode
 */
class SYL_EXPORT ThreadedFirFilter : public QObject, public FirFilter
{
    Q_OBJECT
public:
    /** Creates a new FIR filter. The number of filter taps is extended to the
     * next power of 2.
     *
     * @param taps filter taps
     * @param synchronous @c true if the filter should operate synchronously,
     * i.e., if the frame number to be processed is always an integer multiple
     * of the filter length; otherwise a delay of @a length frames is introduced
     * @param parent parent object that should take ownership or @c NULL
     */
    ThreadedFirFilter(const QVector<double> &taps, bool synchronous,
                      QObject *parent = NULL);

public Q_SLOTS:
    /** Processes a certain amount of data. If @a synchronous was specified @c
     * true at construction, the length of the data must be an integer multiple
     * of #length().
     *
     * @param data data to be filtered, will also contain the output data
     * @param dataLength number of frames to filter, if @a synchronous was
     * specified @c true at construction, this must be an integer multiple of
     * #length()
     * @param semaphore if not @c NULL, QSemaphore#release() will be called on
     * completion
     */
    void process(double *data, unsigned int dataLength, QSemaphore *semaphore);
};

} // namespace syl

#endif
