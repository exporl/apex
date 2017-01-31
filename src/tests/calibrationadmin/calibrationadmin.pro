CLEBS *= apexcaladmin apextools asciicast
TARGET = calibrationadmintest

!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

QT *= testlib gui widgets
