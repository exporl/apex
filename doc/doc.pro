BASEDIR = ../
include($$BASEDIR/clebs.pri)

TEMPLATE = subdirs

doc.files= changelog.txt
doc.path = $$DOCDIR

INSTALLS *= doc
