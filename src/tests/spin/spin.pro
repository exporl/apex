CLEBS *= apextools apexdata apexmain apexspin common asciicast
TARGET = spintest

!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

QT *= testlib gui widgets
