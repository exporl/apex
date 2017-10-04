#ifndef _APEX_SRC_TESTS_CALIBRATIONADMIN_CALIBRATIONADMINTEST_H_
#define _APEX_SRC_TESTS_CALIBRATIONADMIN_CALIBRATIONADMINTEST_H_

#include <QtTest/QtTest>

class CalibrationAdminTest : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase();
    void init();
    void cleanup();

    void testMakeLocalLocal();
    void testMakeGlobalLocal();
    void testMakeInvalidLocal();

    void testMakeLocalGlobal();
    void testMakeGlobalGlobal();
    void testMakeInvalidGlobal();

    void testHardwareSetup();

private:
    struct Setup {
        QString name;
        enum Location { LOCAL, GLOBAL } location;
    } localSetup, globalSetup, invalidSetup;

    void removeSetup(Setup &s);
    void addSetup(Setup &s);
};

#endif // _APEX_SRC_TESTS_CALIBRATIONADMIN_CALIBRATIONADMINTEST_H_
