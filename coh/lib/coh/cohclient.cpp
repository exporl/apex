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

#include "common/exception.h"
#include "common/pluginloader.h"

#include "cohclient.h"

#include <QStringList>

using namespace cmn;

namespace coh
{

CohClient *CohClient::createCohClient(const QString &device)
{
    QList<CohClientCreator *> drivers =
        PluginLoader().availablePlugins<CohClientCreator>();

    const QString driverName = device.section(QRegExp(QL1S(": ?")), 0, 0);
    const QString deviceName = device.section(QRegExp(QL1S(": ?")), 1);

    CohClientCreator *creator = NULL;
    Q_FOREACH (auto *driver, drivers) {
        if (driver->cohDriverName() == driverName) {
            creator = driver;
            break;
        }
    }

    if (!creator)
        throw Exception(
            tr("CI driver '%1' not found").arg(QString(driverName)));

    const QStringList devices = creator->cohDevices();
    if (devices.isEmpty() && deviceName.isEmpty())
        throw Exception(tr("Unable to instantiate default CI device"));

    return creator->createCohClient(
        !devices.isEmpty() && deviceName.isEmpty() ? devices[0] : deviceName);
}

QStringList CohClient::allCohDevices()
{
    QStringList result;

    QList<CohClientCreator *> drivers(
        PluginLoader().availablePlugins<CohClientCreator>());
    Q_FOREACH (auto *driver, drivers) {
        const QStringList devices = driver->cohDevices();
        Q_FOREACH (const auto &device, devices)
            result.append(QString::fromLatin1("%1: %2").arg(
                driver->cohDriverName(), device));
    }

    return result;
}

} // namespace coh
