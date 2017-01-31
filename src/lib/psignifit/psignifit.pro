BASEDIR = ../../..
CLEBS += builddll 

DEFINES *= PSIGNIFIT_MAKEDLL

include($$BASEDIR/clebs.pri)

TARGET = psignifit

PFDIR=$$BASEDIR/contrib/psignifit
INCLUDEPATH *= $$PFDIR

SOURCES += \
    $$PFDIR/adaptivestubs.c \
    $$PFDIR/batchfiles.c \
    $$PFDIR/fitprefs.c \
    $$PFDIR/matlabtools.c \
    $$PFDIR/matrices.c \
    $$PFDIR/priors.c \
    $$PFDIR/psignifit.c \
    $$PFDIR/psychometric.c \
    $$PFDIR/supportfunctions.c \
    psignifitwrapper.cpp

HEADERS += \
   $$PFDIR/adaptiveinterface.h \
    $$PFDIR/batchfiles.h \
    $$PFDIR/mathheader.h \
    $$PFDIR/matlabtools.h \
    $$PFDIR/matrices.h \
    $$PFDIR/priors.h \
    $$PFDIR/psignifit.h \
    $$PFDIR/psychometric.h \
    $$PFDIR/supportfunctions.h \
    $$PFDIR/universalprefix.h \
    psignifitwrapper.h

