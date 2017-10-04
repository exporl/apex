CLEBS *= common builddll +protobuf
TARGET = exporlcoh
!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

QT *= xml

DEFINES *= COH_MAKEDLL

PROTOS = ../../data/protocoh.proto
