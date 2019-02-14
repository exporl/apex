CLEBS *= common commongui asciicast
TARGET = javascripttests

!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

QT *= testlib websockets
