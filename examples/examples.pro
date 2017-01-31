BASEDIR = ..

include ($$BASEDIR/clebs.pri)

TEMPLATE = subdirs

examples.path = $$DATADIR/examples
examples.files = *
#examples.files = `find -name *.xml -or -name *.apx`

INSTALLS *= examples
