!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

TEMPLATE = subdirs

testdata.path = $$DATADIR/testdata
testdata.files = *.wav *.html

INSTALLS *= testdata
