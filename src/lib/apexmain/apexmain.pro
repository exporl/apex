CLEBS *= +xerces coh streamapp apextools apexdata apexwriters portaudio psignifit -ctags subdirsources pch asciicast builddll
unix_CLEBS *= jack

DEFINES *= APEX_MAKEDLL

!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

QT += gui network printsupport script scripttools svg webkitwidgets xml xmlpatterns

lessThan(QT_MAJOR_VERSION, 5):QT += webkit

DEFINES += SETMIXER

win32 {
    QT += axcontainer

    # Always compile flash support
    DEFINES += FLASH
    # Fix link order problems
    DEFINES += _AFXDLL

    # Windows header file version
    DEFINES += WINVER=0x500
} else {
    # TODO move platform-dependent code to plugin, this is ugly
    HEADERS -= gui/flashwidget.h
    SOURCES -= gui/flashwidget.cpp
    HEADERS -= screen/flashplayerrundelegate.h
    SOURCES -= screen/flashplayerrundelegate.cpp
}

android {
    QT -= webkitwidgets
    HEADERS -= \
        result/resultviewer.h \
        result/webpageprocessor.h \
        resultsink/rtresultsink.h \
        runner/flowrunner.h \
        runner/flowapi.h \
        gui/flashwidget.h \
        screen/htmlrundelegate.h \
        screen/flashplayerrundelegate.h
    SOURCES -= \
        result/resultviewer.cpp \
        result/webpageprocessor.cpp \
        resultsink/rtresultsink.cpp \
        runner/flowrunner.cpp \
        runner/flowapi.cpp \
        gui/flashwidget.cpp \
        screen/htmlrundelegate.cpp \
        screen/flashplayerrundelegate.cpp
}

# TODO these are still there but not used?
SOURCES -= pa5device/pa5_actx.cpp

FORMS += \
    gui/autocalibrationdialog.ui \
    gui/calibrationdialog.ui \
    gui/calibrationsetupsdialog.ui \
    gui/connectiondialog.ui \
    gui/mapwizard.ui \
    gui/parameterdialog.ui \
    gui/plugindialog.ui \
    gui/r216settings.ui \
    gui/resultviewer.ui \
    gui/soundcarddialog.ui \
    gui/soundcardsdialog.ui \
    gui/startupdialog.ui \

CTAGSSRCDIRS = $${BASEDIR}/contrib $${BASEDIR}/mwffilter $${BASEDIR}/src $${BASEDIR}/coh $${BASEDIR}/common
