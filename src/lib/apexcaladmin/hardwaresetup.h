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
#ifndef _APEX_SRC_LIB_APEXCALADMIN_HARDWARESETUP_H_
#define _APEX_SRC_LIB_APEXCALADMIN_HARDWARESETUP_H_

#include "apextools/global.h"

#include "namelist.h"

#include <QStringList>

class HardwareSetup;

typedef NameList<HardwareSetup> HardwareSetups;

class CALIBRATIONADMIN_EXPORT HardwareSetup
{
public:
    HardwareSetup();
    bool isLocal() const;
    bool isGlobal() const;
    QString name() const;

    bool operator==(const HardwareSetup &other) const;

    QStringList profiles() const;

private:
    explicit HardwareSetup(const QString &name, bool isLocal);

    QString name_;
    bool isLocal_;

    friend class CalibrationDatabase;
};

#endif // _APEX_SRC_LIB_APEXCALADMIN_HARDWARESETUP_H_
