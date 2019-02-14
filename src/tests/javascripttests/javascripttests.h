#ifndef _APEX_SRC_TESTS_JAVASCRIPTTESTS_JAVASCRIPTTESTS_H_
#define _APEX_SRC_TESTS_JAVASCRIPTTESTS_JAVASCRIPTTESTS_H_

#include <QObject>

class JavaScriptTests : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testScript_data();
    void testScript();
};

#endif