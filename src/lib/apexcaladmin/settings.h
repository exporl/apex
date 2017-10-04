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
#ifndef _APEX_SRC_LIB_APEXCALADMIN_SETTINGS_H_
#define _APEX_SRC_LIB_APEXCALADMIN_SETTINGS_H_

#include "apextools/global.h"

#include <QSettings>

struct CALIBRATIONADMIN_EXPORT DatabaseNames {
    static const QString prefix;
    static const QString hardwareSetup;
    static const QString currentHardwareSetup;

    static QString prepare(const QString &value);

    static QString data(const QString &setup = QString(),
                        const QString &profile = QString(),
                        const QString &name = QString(),
                        const QString &key = QString());
};

class CALIBRATIONADMIN_EXPORT Settings
{
public:
    QSettings *local();
    QSettings *global();
};

#endif // _APEX_SRC_LIB_APEXCALADMIN_SETTINGS_H_
