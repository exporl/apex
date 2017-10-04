CLEBS *= common
TARGET = commontest
!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

QT += testlib network websockets

# TODO: this should not be necessary
DEFINES -= QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII
