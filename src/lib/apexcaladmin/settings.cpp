/******************************************************************************
 * Copyright (C) 2014  Jonas Vanthornhout <jonasvanthornhout+apex@gmail.com>  *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/
#include "settings.h"

const QString DatabaseNames::prefix = "Calibration";
const QString DatabaseNames::hardwareSetup =
    DatabaseNames::prefix + "/config/hardware";
const QString DatabaseNames::currentHardwareSetup =
    DatabaseNames::prefix + "/config/current";

QString DatabaseNames::prepare(const QString &value)
{
    return QString(value).replace('/', '_');
}

QString DatabaseNames::data(const QString &setup, const QString &profile,
                            const QString &name, const QString &key)
{
    QString result(prefix + "/data");
    if (setup.isEmpty())
        return result;
    result += "/" + prepare(setup);
    if (profile.isEmpty())
        return result;
    result += "/" + prepare(profile);
    if (name.isEmpty())
        return result;
    result += "/" + prepare(name);
    if (key.isEmpty())
        return result;
    result += "/" + prepare(key);
    return result;
}

QSettings *Settings::local()
{
    static QScopedPointer<QSettings> localSettings(
        new QSettings(QSettings::UserScope, QSettings().organizationName(),
                      QSettings().applicationName()));
    localSettings->setFallbacksEnabled(false);
    return localSettings.data();
}

QSettings *Settings::global()
{
    static QScopedPointer<QSettings> globalSettings(
        new QSettings(QSettings::SystemScope, QSettings().organizationName(),
                      QSettings().applicationName()));
    globalSettings->setFallbacksEnabled(false);
    return globalSettings.data();
}
