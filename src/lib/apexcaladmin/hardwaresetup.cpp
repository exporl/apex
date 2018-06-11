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
#include "hardwaresetup.h"
#include "settings.h"
#include <QSettings>

HardwareSetup::HardwareSetup()
{
}
HardwareSetup::HardwareSetup(const QString &name, bool isLocal)
    : name_(name), isLocal_(isLocal)
{
}

QString HardwareSetup::name() const
{
    return name_;
}

bool HardwareSetup::isGlobal() const
{
    return !isLocal();
}

bool HardwareSetup::isLocal() const
{
    return isLocal_;
}

bool HardwareSetup::operator==(const HardwareSetup &other) const
{
    return name() == other.name() && isGlobal() == other.isGlobal();
}

QStringList HardwareSetup::profiles() const
{
    QSettings *settings = 0;

    // It is impossible to have a hardware setup that is global and local
    if (isGlobal()) {
        settings = Settings().global();
    } else if (isLocal()) {
        settings = Settings().local();
    }

    settings->beginGroup(DatabaseNames::data(name()));
    QStringList result = settings->childGroups();
    settings->endGroup();

    return result;
}
