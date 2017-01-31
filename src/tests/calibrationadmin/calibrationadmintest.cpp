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

#include "apexcaladmin/calibrationdatabase.h"
#include "apexcaladmin/hardwaresetup.h"
#include "apexcaladmin/settings.h"

#include "apextools/global.h"

#include "calibrationadmintest.h"

void CalibrationAdminTest::init() {
    CalibrationDatabase cd;
    HardwareSetups locals = cd.localHardwareSetups();
    HardwareSetups globals = cd.globalHardwareSetups();

    localSetup.name = "testLocal";
    while(locals.contains(localSetup.name)) {
        localSetup.name += "0";
    }
    localSetup.location = Setup::LOCAL;

    globalSetup.name = "testGlobal";
    while(globals.contains(globalSetup.name)) {
        globalSetup.name += "0";
    }
    globalSetup.location = Setup::GLOBAL;

    invalidSetup.name = "testInvalid";
    while(globals.contains(invalidSetup.name) || locals.contains(invalidSetup.name)) {
        invalidSetup.name += "0";
    }

    addSetup(localSetup);
    addSetup(globalSetup);
}

void CalibrationAdminTest::addSetup(Setup& s) {
    QSettings* settings = NULL;

    if(s.location == Setup::LOCAL) {
        settings = Settings().local();
    } else {
        settings = Settings().global();
    }

    QStringList newSetups = settings->value(DatabaseNames::hardwareSetup).toStringList();
    newSetups.append(s.name);
    settings->setValue(DatabaseNames::hardwareSetup, newSetups);

    settings->setValue(DatabaseNames::data(s.name, "profile1", "param", "key"), 2);

    settings->sync();
}

void CalibrationAdminTest::removeSetup(Setup& s) {
    QSettings* settings = NULL;

    if(s.location == Setup::LOCAL) {
        settings = Settings().local();
    } else {
        settings = Settings().global();
    }

    QStringList setups = settings->value(DatabaseNames::hardwareSetup).toStringList();
    setups.removeOne(s.name);
    if(setups.isEmpty()) {
        settings->remove(DatabaseNames::hardwareSetup);
    } else {
        settings->setValue(DatabaseNames::hardwareSetup, setups);
    }

    settings->beginGroup(DatabaseNames::data(s.name));
    settings->remove("profile1");
    settings->endGroup();

    settings->sync();
}

void CalibrationAdminTest::cleanup() {
    removeSetup(localSetup);
    removeSetup(globalSetup);
}

void CalibrationAdminTest::testMakeGlobalLocal() {
#ifdef Q_OS_UNIX
    QSKIP("Unable to access global calib profiles on unix");
#endif
    //Set initial values
    CalibrationDatabase cd;
    int amountSetups = cd.hardwareSetups().count();
    int amountLocalSetups = cd.localHardwareSetups().count();
    int amountGlobalSetups = cd.globalHardwareSetups().count();
    QSettings* localSettings = Settings().local();
    QSettings* globalSettings = Settings().global();

    //Move the hardware setup
    cd.makeLocal(cd.setup(globalSetup.name));
    globalSetup.location = Setup::LOCAL;

    //Check if the hardware setup is moved
    QCOMPARE(cd.hardwareSetups().count(), amountSetups);
    QCOMPARE(cd.localHardwareSetups().count(), amountLocalSetups + 1);
    QCOMPARE(cd.globalHardwareSetups().count(), amountGlobalSetups - 1);
    QVERIFY(cd.localHardwareSetups().contains(globalSetup.name));
    QVERIFY(!cd.globalHardwareSetups().contains(globalSetup.name));

    //Check if the profile is moved
    localSettings->beginGroup(DatabaseNames::data(globalSetup.name));
    QCOMPARE(localSettings->childGroups().count(), 1);
    localSettings->endGroup();

    globalSettings->beginGroup(DatabaseNames::data(globalSetup.name));
    QCOMPARE(globalSettings->childGroups().count(), 0);
    globalSettings->endGroup();
}

void CalibrationAdminTest::testMakeLocalLocal() {
    CalibrationDatabase cd;
    try {
        cd.makeLocal(cd.setup(localSetup.name));
        QFAIL("Making a local hardware setup local should fail.");
    } catch(const std::domain_error&) {
        // expected
    } catch(...) {
        QFAIL("makeLocal throws the wrong error.");
    }
}

void CalibrationAdminTest::testMakeInvalidLocal() {
    CalibrationDatabase cd;
    try {
        cd.makeLocal(cd.setup(invalidSetup.name));
        QFAIL("Making a invalid hardware setup local should fail.");
    } catch(const std::range_error&) {
        // expected
    } catch(...) {
        QFAIL("makeLocal throws the wrong error.");
    }
}

void CalibrationAdminTest::testMakeLocalGlobal() {
#ifdef Q_OS_UNIX
    QSKIP("Unable to access global calib profiles on unix");
#endif
    //Set initial values
    CalibrationDatabase cd;
    int amountSetups = cd.hardwareSetups().count();
    int amountLocalSetups = cd.localHardwareSetups().count();
    int amountGlobalSetups = cd.globalHardwareSetups().count();
    QSettings* localSettings = Settings().local();
    QSettings* globalSettings = Settings().global();

    //Move the hardware setup
    cd.makeGlobal(cd.setup(localSetup.name));
    localSetup.location = Setup::GLOBAL;

    //Check if the hardware setup is moved
    QCOMPARE(cd.hardwareSetups().count(), amountSetups);
    QCOMPARE(cd.localHardwareSetups().count(), amountLocalSetups - 1);
    QCOMPARE(cd.globalHardwareSetups().count(), amountGlobalSetups + 1);
    QVERIFY(!cd.localHardwareSetups().contains(localSetup.name));
    QVERIFY(cd.globalHardwareSetups().contains(localSetup.name));

    //Check if the profile is moved
    localSettings->beginGroup(DatabaseNames::data(globalSetup.name));
    QCOMPARE(localSettings->childGroups().count(), 0);
    localSettings->endGroup();

    globalSettings->beginGroup(DatabaseNames::data(globalSetup.name));
    QCOMPARE(globalSettings->childGroups().count(), 1);
    globalSettings->endGroup();
}

void CalibrationAdminTest::testMakeGlobalGlobal() {
    CalibrationDatabase cd;
    try {
        cd.makeGlobal(cd.setup(globalSetup.name));
        QFAIL("Making a gloabl hardware setup local should fail.");
    } catch(const std::domain_error&) {
        // expected
    } catch(...) {
        QFAIL("makeLocal throws the wrong error.");
    }
}

void CalibrationAdminTest::testMakeInvalidGlobal() {
    CalibrationDatabase cd;
    try {
        cd.makeGlobal(cd.setup(invalidSetup.name));
        QFAIL("Making a invalid hardware setup local should fail.");
    } catch(const std::range_error&) {
        // expected
    } catch(...) {
        QFAIL("makeLocal throws the wrong error.");
    }
}

void CalibrationAdminTest::testHardwareSetup() {
    CalibrationDatabase cd;
    {
        QCOMPARE(cd.setup(localSetup.name).name(), localSetup.name);
        QVERIFY(!cd.setup(localSetup.name).isGlobal());
        QVERIFY(cd.setup(localSetup.name).isLocal());

        QStringList localProfiles = cd.setup(localSetup.name).profiles();
        QCOMPARE(localProfiles, QStringList("profile1"));
    }

    {
        QCOMPARE(cd.setup(globalSetup.name).name(), globalSetup.name);
        QVERIFY(cd.setup(globalSetup.name).isGlobal());
        QVERIFY(!cd.setup(globalSetup.name).isLocal());

        QStringList globalProfiles = cd.setup(globalSetup.name).profiles();
        QCOMPARE(globalProfiles, QStringList("profile1"));
    }

    QVERIFY((cd.setup(localSetup.name) == cd.setup(localSetup.name)));
    QVERIFY(!(cd.setup(localSetup.name) == cd.setup(globalSetup.name)));
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setOrganizationName(apex::organizationName);
    app.setApplicationName(apex::applicationName);
    app.setOrganizationDomain (apex::organizationDomain);

    CalibrationAdminTest tc;

    try {
        return QTest::qExec(&tc, argc, argv);
    } catch(...) {

    }
}
