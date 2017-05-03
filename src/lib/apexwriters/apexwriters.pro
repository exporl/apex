CLEBS *= apextools apexdata asciicast builddll

!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

QT += gui widgets xml network

DEFINES *= APEXWRITERS_MAKEDLL
