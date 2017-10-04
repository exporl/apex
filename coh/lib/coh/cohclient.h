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
 * CI client interface declaration.
 */

#ifndef _EXPORL_COH_LIB_COH_COHCLIENT_H_
#define _EXPORL_COH_LIB_COH_COHCLIENT_H_

#include "coh.h"

#include <QCoreApplication>
#include <QString>

namespace coh
{

/** Abstract CI client interface. Provides methods to control an implant and to
 * obtain the status of the device.
 *
 * @see CohClientCreator, #create()
 *
 * @ingroup ci
 */
class COH_EXPORT CohClient
{
    Q_DECLARE_TR_FUNCTIONS(CohClient)
public:
    /** Virtual destructor to make the compiler happy. */
    virtual ~CohClient()
    {
    }

    /** Sends a CI command sequence to the implant. Use #start() to start the
     * sequence.
     *
     * @param sequence CI command sequence to send
     *
     * @throws std::exception on errors
     */
    virtual void send(CohSequence *sequence) = 0;

    /** Starts the playback of a CI command sequence sent with #send(). The
     * status of the device will change to abr#CohIdle when the playback
     * is finished.
     *
     * @param trigger trigger mode
     *
     * @throws std::exception on errors
     */
    virtual void start(Coh::TriggerMode trigger) = 0;

    /** Stops the playback of a CI command sequence.
     *
     * @throws std::exception on errors
     */
    virtual void stop() = 0;

    /** Returns the status of the implant.
     *
     * @return implant status
     *
     * @throws std::exception on errors
     */
    virtual Coh::Status status() const = 0;

    /** Returns whether the client must be reloaded to be sure stimulation
     * actually stops.
     *
     * This is a bug-fix for NIC 2 where a very long stimulus results in
     * a non-responsive L34 that can only be stopped by a reload of the driver
     * library.
     *
     * This can be removed when support for NIC 2 is removed.
     *
     * @return whether another client with the same device must be instantiated
     *
     * @throws std::exception on errors
     */
    virtual bool needsReloadForStop() const = 0;

    /** Creates a new CI interface instance. An std::exception is thrown if the
     * driver for the device name can not be found, or if the driver is
     * unable to setup the device. The caller is responsible of freeing the
     * returned pointer.
     *
     * @param device device name as obtained from #devices()
     * @return CI driver instance
     *
     * @throws std::exception on errors
     */
    static CohClient *createCohClient(const QString &device);

    /** Returns the list of devices that are known to all CI drivers. These
     * device names differ from the names obtained from
     * CohClientCreator#ciDevices() by a prefix that distinguishes the
     * different CI drivers
     *
     * @return device names in the form "driver: device"
     */
    static QStringList allCohDevices();
};

/** Abstract CI client creator interface. Provides methods to instantiate
 * CI drivers of a certain type for a specified device and to get a
 * list of available devices.
 *
 * @see CohClient
 *
 * @ingroup ci
 */
class COH_EXPORT CohClientCreator
{
public:
    /** Virtual destructor to make the compiler happy. */
    virtual ~CohClientCreator()
    {
    }

    /** Creates a new CI interface instance for a given device. An
     * std::exception is thrown if the driver is unable to setup the device.
     * The caller is responsible of freeing the returned pointer.
     *
     * @param device device name as obtained from #ciDevices()
     * @return CI driver instance
     *
     * @throws std::exception on errors
     */
    virtual CohClient *createCohClient(const QString &device) = 0;

    /** Returns the list of devices that are known to this CI driver. A
     * CI driver may accept more devices than returned by this method.
     *
     * @return device names
     */
    virtual QStringList cohDevices() const = 0;

    virtual QString cohDriverName() const = 0;
};

} // namespace coh

Q_DECLARE_INTERFACE(coh::CohClientCreator, "be.labexporl.coh.CohClient/1.0")

#endif
