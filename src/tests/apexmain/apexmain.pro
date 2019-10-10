CLEBS *= apexmain common commongui asciicast sndfile
TARGET = apexmaintest

!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

QT *= testlib gui widgets network xml

!android:DEFINES *= ENABLE_COH