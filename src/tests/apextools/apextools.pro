CLEBS *= apextools common asciicast +openssl
TARGET = apextoolstest

!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

QT *= testlib
