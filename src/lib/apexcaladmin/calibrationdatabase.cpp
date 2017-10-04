/******************************************************************************
 * Copyright (C) 2014  Jonas Vanthornhout <jonasvanthornhout+apex@gmail.com>  *
 *                                                                            *
 * This file is part of APEX 3.                                               *
 *                                                                            *
 * APEX 3 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 3 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 3.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "calibrationdatabase.h"
#include "settings.h"

#include <stdexcept>

CalibrationDatabase::CalibrationDatabase()
{
    localSettings = Settings().local();
    globalSettings = Settings().global();
}

HardwareSetups
CalibrationDatabase::QStringList2HardwareSetups(const QStringList &names,
                                                bool isLocal) const
{
    HardwareSetups setups;

    foreach (QString name, names) {
        setups.push_back(HardwareSetup(name, isLocal));
    }

    return setups;
}

QStringList CalibrationDatabase::hardwareSetupNames(QSettings *settings) const
{
    return settings->value(DatabaseNames::hardwareSetup).toStringList();
}

HardwareSetups CalibrationDatabase::localHardwareSetups() const
{
    return QStringList2HardwareSetups(hardwareSetupNames(localSettings), true);
}

HardwareSetups CalibrationDatabase::globalHardwareSetups() const
{
    return QStringList2HardwareSetups(hardwareSetupNames(globalSettings),
                                      false);
}

HardwareSetups CalibrationDatabase::hardwareSetups() const
{
    HardwareSetups both(localHardwareSetups());
    both.append(globalHardwareSetups());
    return both;
}

HardwareSetup CalibrationDatabase::setup(const QString &hardwareSetupName) const
{
    return hardwareSetups().get(hardwareSetupName);
}

bool CalibrationDatabase::isWritable() const
{
    return localSettings->isWritable() && globalSettings->isWritable();
}

bool CalibrationDatabase::exists(QString hardwareSetupName) const
{
    return hardwareSetups().contains(hardwareSetupName);
}

void CalibrationDatabase::makeGlobal(HardwareSetup hardwareSetup)
{
    if (hardwareSetup.isGlobal()) {
        throw std::domain_error(tr("The hardware setup %1 is already global")
                                    .arg(hardwareSetup.name())
                                    .toStdString());
    }

    move(hardwareSetup.name(), localSettings, globalSettings);
}

void CalibrationDatabase::makeLocal(HardwareSetup hardwareSetup)
{
    if (hardwareSetup.isLocal()) {
        throw std::domain_error(tr("The hardware setup %1 is already local")
                                    .arg(hardwareSetup.name())
                                    .toStdString());
    }

    move(hardwareSetup.name(), globalSettings, localSettings);
}

void CalibrationDatabase::move(const QString &hardwareSetupName,
                               QSettings *from, QSettings *to)
{
    if (!isWritable()) {
        throw std::domain_error(tr("Can't write the settings, are you running "
                                   "this program as administrator?")
                                    .toStdString());
    }

    // Add setup
    QStringList newHardwareSetupsTo =
        to->value(DatabaseNames::hardwareSetup).toStringList();
    newHardwareSetupsTo.append(hardwareSetupName);
    to->setValue(DatabaseNames::hardwareSetup, newHardwareSetupsTo);

    // Remove setup
    QStringList newHardwareSetupsFrom =
        from->value(DatabaseNames::hardwareSetup).toStringList();
    newHardwareSetupsFrom.removeOne(hardwareSetupName);
    if (newHardwareSetupsFrom.isEmpty()) {
        from->remove(DatabaseNames::hardwareSetup);
        from->remove(DatabaseNames::currentHardwareSetup);
    } else {
        from->setValue(DatabaseNames::hardwareSetup, newHardwareSetupsFrom);
    }

    // move profiles and their params
    from->beginGroup(DatabaseNames::data(hardwareSetupName));
    QStringList keys = from->allKeys();
    from->endGroup();

    foreach (QString key, keys) {
        to->beginGroup(DatabaseNames::data(hardwareSetupName));
        from->beginGroup(DatabaseNames::data(hardwareSetupName));

        to->setValue(key, from->value(key));
        from->remove(key);

        from->endGroup();
        to->endGroup();
    }

    // delete profile
    from->remove(DatabaseNames::data(hardwareSetupName));

    from->sync();
    to->sync();
}
