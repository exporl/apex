CLEBS *= apextools apexmain asciicast

!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

QT *= core gui
greaterThan(QT_MAJOR_VERSION, 4):QT *= widgets
