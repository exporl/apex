CLEBS *= streamapp apextools apexdata apexwriters portaudio -ctags subdirsources asciicast builddll commongui sndfile bertha +libgit2
!android:CLEBS += coh
unix_CLEBS *= -jack

DEFINES *= APEX_MAKEDLL
!android:DEFINES *= ENABLE_COH

!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

QT += gui network printsupport script scripttools svg xml xmlpatterns

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
    QT += androidextras

    HEADERS -= \
        gui/flashwidget.h \
        screen/flashplayerrundelegate.h
    SOURCES -= \
        gui/flashwidget.cpp \
        screen/flashplayerrundelegate.cpp \
        cohstimulus/cohdatablock.cpp \
        cohstimulus/cohdevice.cpp \
        device/cohdeviceparser.cpp \
        map/mapfactory.cpp
} else {
    HEADERS -= apexandroidnative.h
    SOURCES -= apexandroidnative.cpp
}

FORMS += \
    gui/autocalibrationdialog.ui \
    gui/calibrationdialog.ui \
    gui/calibrationsetupsdialog.ui \
    gui/connectiondialog.ui \
    gui/parameterdialog.ui \
    gui/plugindialog.ui \
    gui/soundcarddialog.ui \
    gui/soundcardsdialog.ui \
    gui/startupdialog.ui \
    gui/studydialog.ui \
    screen/numerickeypadwidget.ui \

CTAGSSRCDIRS = $${BASEDIR}/contrib $${BASEDIR}/mwffilter $${BASEDIR}/src $${BASEDIR}/bertha-lib $${BASEDIR}/coh $${BASEDIR}/common
