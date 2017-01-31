CLEBS *= apextools apexdata apexmain asciicast
TARGET = apexmaintest

!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

QT *= testlib gui webkitwidgets network xml
lessThan(QT_MAJOR_VERSION, 5):QT *= webkit

android {
    QT -= webkitwidgets
    QT += webview
    HEADERS -= apexmaintest.h
    SOURCES -= apexmaintest.cpp
}
