CLEBS *= pch protobuf builddll sndfile
TARGET = exporlcommon
!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

QT *= xml network websockets
android:QT *= androidextras

DEFINES *= COMMON_MAKEDLL

win32:LIBS *= -lshell32

PROTOS = ../../data/protoslave.proto
