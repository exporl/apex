CLEBS *= protobuf builddll common
TARGET = exporlcommonprotobuf
!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

QT *= network

DEFINES *= COMMON_MAKEDLL

PROTOS = ../../data/protoslave.proto
