BASEDIR = ../../..
CLEBS += xerces builddll apextools apexdata

DEFINES *= APEXWRITERS_MAKEDLL

include($$BASEDIR/clebs.pri)

QT *= gui

TARGET = apexwriters

HEADERS += \
    experimentwriter.h \
    deviceswriter.h \
    datablockswriter.h \
    procedureswriter.h \
    fileprefixwriter.h \
    filterswriter.h \
    screenswriter.h \
    stimuliwriter.h \
    correctorwriter.h \
    connectionswriter.h \
    resultparameterswriter.h \
    generalparameterswriter.h \
    calibrationwriter.h \

SOURCES += \
    experimentwriter.cpp \
    deviceswriter.cpp \
    datablockswriter.cpp \
    procedureswriter.cpp \
    fileprefixwriter.cpp \
    filterswriter.cpp \
    screenswriter.cpp \
    stimuliwriter.cpp \
    correctorwriter.cpp \
    connectionswriter.cpp \
    resultparameterswriter.cpp \
    generalparameterswriter.cpp \
    calibrationwriter.cpp \
