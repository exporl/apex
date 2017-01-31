!isEmpty(_PRO_FILE_):include(../clebs/clebs.pri)

TEMPLATE = subdirs

unix: examples.commands = ../tools/documentation-examples-text.sh >index.txt
examples.path = $$DATADIR/examples
examples.files = *
INSTALLS *= examples
