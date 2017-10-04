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

#include "common/exception.h"

#include "devicedata.h"
#include "deviceinterface.h"
#include "deviceprocessor.h"

using namespace cmn;

namespace bertha
{

// DeviceProcessor =============================================================

void DeviceProcessor::deviceCallback(void *device)
{
    Q_EMIT static_cast<DeviceProcessor *>(device)->callback();
}

DeviceProcessor::DeviceProcessor(const DeviceData &data)
    : AbstractProcessor(data, data.blockSize(), data.sampleRate(),
                        availableDevicePlugins())
{
    device = castPlugin<DeviceInterface>();
    device->setCallback(deviceCallback, this);
}

DeviceProcessor::~DeviceProcessor()
{
}

void DeviceProcessor::prepare()
{
    const QString errorMessage = device->prepare();
    if (!errorMessage.isEmpty())
        throw Exception(
            tr("Unable to prepare device %1: %2").arg(id(), errorMessage));
}

void DeviceProcessor::start()
{
    const QString errorMessage = device->start();
    if (!errorMessage.isEmpty())
        throw Exception(
            tr("Unable to start device %1: %2").arg(id(), errorMessage));
}

bool DeviceProcessor::processOutput()
{
    return device->processOutput();
}

bool DeviceProcessor::processInput()
{
    return device->processInput();
}

void DeviceProcessor::stop()
{
    device->stop();
}

void DeviceProcessor::release()
{
    device->release();
}

void DeviceProcessor::connectCallback(QObject *receiver, const char *method)
{
    QObject::connect(this, SIGNAL(callback()), receiver, method,
                     Qt::DirectConnection);
}

QMap<QString, BlockDescription> DeviceProcessor::availableDevicePlugins()
{
    return availableCreatorPlugins<DeviceCreatorInterface>();
}

DeviceInterface *DeviceProcessor::deviceInterface() const
{
    return device;
}

} // namespace bertha
