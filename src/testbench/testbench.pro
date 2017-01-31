BASEDIR = ../..
CLEBS *= apextools apexmain apexdata apexwriters
TEMPLATE = subdirs
include($$BASEDIR/clebs.pri)

SUBDIRS = \
    app/ \
    apexdata/ \
    apexwriters \
    apextools/ \
    spin/ \
    apexmain/ \
    
