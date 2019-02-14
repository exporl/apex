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

#ifndef _APEX_SRC_LIB_APEXTOOLS_SETTINGSFACTORY_H_
#define _APEX_SRC_LIB_APEXTOOLS_SETTINGSFACTORY_H_

#include "global.h"

#include <QSettings>

/*
 * SettingsFactory can be used to create QSettings instances that are
 * configured to load and persist their state to the correct location.
 */

namespace apex
{

class APEXTOOLS_EXPORT SettingsFactory
{
public:
    static void initializeForUserSpecificSettings();
    static void initializeForInstallationWideSettings();
    static void initialize(QString path);
    QSettings *createSettings();

    static QSettings::Format format;
    static QSettings::Scope scope;
};
}

#endif
