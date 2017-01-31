BASEDIR = ../../..
CLEBS += xerces \
    builddll \
    globalincludes \
	xalan
DEFINES *= APEXTOOLS_MAKEDLL
include ($$BASEDIR/clebs.pri)
win32:LIBS *= -lshell32 # ShellExecute
QT += gui \
    script \
    svg \
    xml \
    webkit \
    network \
	xmlpatterns
QT += qt3support
	
HEADERS += global.h \
    apextools.h \
    apexrandom.h \
    exceptions.h \
    apextypedefs.h \
    random.h \
    pathtools.h \
    xml/apexxmltools.h \
    xml/xmldocumentgetter.h \
    xml/xmlkeys.h \
    gui/arclayout.h \
    status/errorlogger.h \
    status/statusitem.h \
    status/statusreporter.h \
    status/statusdispatcher.h \
    status/consolestatusreporter.h \
    status/screenstatusreporter.h \
    status/statuswindow.h \
    signalprocessing/iirfilter.h \
    signalprocessing/peakfilter.h \
    optionutils.h \
    desktoptools.h \
    programoptions/optioncontext.h \
    programoptions/optionparser.h \
    xml/xercesinclude.h \
    xml/xalaninclude.h \
    services/servicemanager.h \
    services/application.h \
    services/paths.h

SOURCES += apextools.cpp \
    random.cpp \
    pathtools.cpp \
    xml/apexxmltools.cpp \
    xml/xmldocumentgetter.cpp \
    gui/arclayout.cpp \
    status/errorlogger.cpp \
    status/statusitem.cpp \
    status/statusreporter.cpp \
    status/statusdispatcher.cpp \
    status/consolestatusreporter.cpp \
    status/screenstatusreporter.cpp \
    status/statuswindow.cpp \
    exceptions.cpp \
    signalprocessing/iirfilter.cpp \
    signalprocessing/peakfilter.cpp \
    optionutils.cpp \
    desktoptools.cpp \
    programoptions/optionparser.cpp \
    programoptions/optioncontext.cpp \
    services/servicemanager.cpp \
    services/application.cpp \
    services/paths.cpp

