BASEDIR = ../..
CLEBS *= pdflatex
include($$BASEDIR/clebs.pri)

manual.files= apexpaper.pdf referencemanual.pdf 
manual.path = $$DOCDIR

contains(CLEBS_DEPENDENCIES, pdflatex) {
    unix:QMAKE_LINK = true
    win32:QMAKE_LINK = rem

    DESTDIR = .
    OBJECTS_DIR = 

    ALL_DEPS += apex3manual.pdf
    PDFLATEX_SOURCE += apex3manual.tex


    manual.files *= apex3manual.pdf
    
} else {
    DESTDIR = .
    TEMPLATE = subdirs
}

INSTALLS *= manual
INSTALLS -= target
