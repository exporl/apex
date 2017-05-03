CLEBS *= apexmain apextools apexwriters apexdata common asciicast builddll

!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

DEFINES += APEXSPIN_MAKEDLL

QT += gui widgets

FORMS += $$files(*.ui)

RESOURCES = resources.qrc

