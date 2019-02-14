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

#include "apextools/settingsfactory.h"
#include "apextoolstest.h"
#include "common/paths.h"
#include "common/testutils.h"

#include <QTest>

using namespace apex;

void ApexToolsTest::settingsFactoryTest_initializeForUserSpecificSettings()
{
    TEST_EXCEPTIONS_TRY

    SettingsFactory::initializeForUserSpecificSettings();
    SettingsFactory factory;

    QSharedPointer<QSettings> settings =
        QSharedPointer<QSettings>(factory.createSettings());

    QCOMPARE(settings->scope(), QSettings::UserScope);
    QCOMPARE(settings->format(), QSettings::NativeFormat);

#ifdef Q_OS_WIN
    QCOMPARE(settings->fileName(),
             QSL("\\HKEY_CURRENT_USER\\Software\\ExpORL\\APEX"));
#else
    QCOMPARE(settings->fileName(),
             QDir::homePath() + "/.config/ExpORL/APEX.conf");
#endif

    TEST_EXCEPTIONS_CATCH
}

void ApexToolsTest::settingsFactoryTest_initializeForInstallationWideSettings()
{
    TEST_EXCEPTIONS_TRY

    SettingsFactory::initializeForInstallationWideSettings();
    SettingsFactory factory;

    QSharedPointer<QSettings> settings =
        QSharedPointer<QSettings>(factory.createSettings());

    QCOMPARE(settings->scope(), QSettings::SystemScope);

#ifdef Q_OS_WIN
    QCOMPARE(settings->fileName(),
             cmn::Paths::installedBaseDirectory() + "/config/ExpORL/APEX.ini");
    QCOMPARE(settings->format(), QSettings::IniFormat);
#else
    QCOMPARE(settings->fileName(),
             cmn::Paths::installedBaseDirectory() + "/config/ExpORL/APEX.conf");
    QCOMPARE(settings->format(), QSettings::NativeFormat);
#endif

    TEST_EXCEPTIONS_CATCH
}
