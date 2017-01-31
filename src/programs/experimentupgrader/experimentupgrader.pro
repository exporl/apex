CLEBS *= +xerces apextools apexdata apexmain asciicast

!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

QT += core gui xml xmlpatterns
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

FORMS += $$files(*.ui)
