CLEBS *= builddll sndfile
TARGET = exporlcommon
!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

QT *= xml websockets webchannel
android:QT *= androidextras

DEFINES *= COMMON_MAKEDLL

win32:LIBS *= -lshell32
