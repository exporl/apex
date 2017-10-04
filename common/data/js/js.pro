!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

TEMPLATE = subdirs

js.path = $$DATADIR/js
js.files = *.js

INSTALLS *= js
