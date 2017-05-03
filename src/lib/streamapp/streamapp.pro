CLEBS *= +portaudio asciicast
win32_CLEBS *= +asio
unix_CLEBS *= -jack

!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

QT += core gui

# set main config
TEMPLATE    = lib
VERSION     = 2.0
CONFIG      += staticlib

contains(CLEBS_DEPENDENCIES, jack) {
    DEFINES *= ENABLEJACK
}

win32:DEFINES -= UNICODE
MAC:DEFINES += MAC
unix:release:DEFINES += NDEBUG

# remove defaults from clebs
HEADERS -= $$files(*.h)
SOURCES -= $$files(*.cpp)

# do not install the static library
INSTALLS -= target

HEADERS += \
    soundcard/bufferdropcheck.h \
    soundcard/dummysoundcard.h \
    soundcard/portaudiowrapper.h \

SOURCES += \
    appcore/threads/readwritelock.cpp \
    appcore/threads/slicedthread.cpp \
    appcore/threads/thread.cpp \
    audioformatconvertor.cpp \
    audioformatstream.cpp \
    audiosamplebuffer.cpp \
    bufferedprocessor.cpp \
    callback/manualcallbackrunner.cpp \
    callback/multicallback.cpp \
    callback/streamappcallbacks.cpp \
    compare.cpp \
    connections.cpp \
    containers/rawmemorybundle.cpp \
    containers/rawmemory.cpp \
    eofcheck.cpp \
    file/binaryfile.cpp \
    file/wavefile.cpp \
    generators/sinegenerator.cpp \
    multiproc.cpp \
    multistream.cpp \
    processors/buffer.cpp \
    processors/gate.cpp \
    processors/matrixmixer.cpp \
    resamplers/resamplingbuffer.cpp \
    silentreader.cpp \
    soundcard/bufferdropcheck.cpp \
    soundcard/dummysoundcard.cpp \
    soundcard/portaudiowrapper.cpp \
    soundcard/soundcardfactory.cpp \
    utils/systemutils.cpp \
    utils/tracer.cpp \
    utils/tracermemory.cpp \

win32:SOURCES += \
    _archs/win32/win32_threads.cpp \
    _archs/win32/win32_cputicker.cpp \
    _archs/win32/win32_ctimer1.cpp \
    _archs/win32/win32_misc.cpp \
    _fromv3/_archs/win32/automation/clsid.cpp \
    _archs/win32/win32_asiowrapper.cpp \

unix:SOURCES += \
    _archs/linux/linux_threads.cpp \
    _archs/linux/linux_misc.cpp \

contains(DEFINES, ENABLEJACK) {
    SOURCES += _archs/linux/linux_jackwrapper.cpp
}

unix:COREAUDIO:SOURCES += _archs/osx/osx_coreaudiowrapper.cpp


