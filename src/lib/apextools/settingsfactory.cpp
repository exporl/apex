/******************************************************************************
 * Copyright (C) 2017 Division of Experimental Otorhinolaryngology K.U.Leuven *
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

#include "settingsfactory.h"
#include "common/paths.h"

#include <QCoreApplication>
#include <QSettings>

namespace apex
{

void SettingsFactory::initializeForUserSpecificSettings()
{
    SettingsFactory::format = QSettings::NativeFormat;
    SettingsFactory::scope = QSettings::UserScope;
}

void SettingsFactory::initializeForInstallationWideSettings()
{
    const QString &path = cmn::Paths::installedBaseDirectory() + QSL("/config");
    initialize(path);
}

void SettingsFactory::initialize(QString path)
{
    QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, path);
    QSettings::setPath(QSettings::NativeFormat, QSettings::SystemScope, path);

#ifdef Q_OS_WIN
    SettingsFactory::format = QSettings::IniFormat;
#else
    SettingsFactory::format = QSettings::NativeFormat;
#endif
    SettingsFactory::scope = QSettings::SystemScope;
}

QSettings *SettingsFactory::createSettings()
{
    return new QSettings(format, scope, apex::organizationName,
                         apex::applicationName);
}

QSettings::Format SettingsFactory::format = QSettings::NativeFormat;
QSettings::Scope SettingsFactory::scope = QSettings::UserScope;
}