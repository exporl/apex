/******************************************************************************
 * Copyright (C) 2008  Michael Hofmann <mh21@piware.de>                       *
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

#ifndef _BERTHA_SRC_LIB_BERTHA_DEVICEPROCESSOR_H_
#define _BERTHA_SRC_LIB_BERTHA_DEVICEPROCESSOR_H_

#include "abstractprocessor.h"
#include "deviceinterface.h"

namespace bertha
{

class DeviceData;

class BERTHA_EXPORT DeviceProcessor : public AbstractProcessor
{
    Q_OBJECT
public:
    DeviceProcessor(const DeviceData &data);
    virtual ~DeviceProcessor();

    // throw
    void prepare();
    void start();
    void stop();
    void release();

    bool processOutput();
    bool processInput();

    // use SLOT() for the methodname
    void connectCallback(QObject *receiver, const char *method);

    /**
      * Return internal pointer to DeviceInterface
      * this should normally not be used, except e.g. in a test-bench
      */
    DeviceInterface *deviceInterface() const;

    static QMap<QString, BlockDescription> availableDevicePlugins();

Q_SIGNALS:
    void callback();

private:
    static void deviceCallback(void *device);

private:
    DeviceInterface *device;
};

}; // namespace bertha

#endif
