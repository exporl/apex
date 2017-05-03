CLEBS *= apexcaladmin apextools common asciicast
TARGET = calibrationadmintest

!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

QT *= testlib gui widgets
