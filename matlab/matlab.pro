BASEDIR = ..

include ($$BASEDIR/clebs.pri)

TEMPLATE = subdirs

matlab.path = $$DATADIR/amt
matlab.files = amt.zip

INSTALLS *= matlab
