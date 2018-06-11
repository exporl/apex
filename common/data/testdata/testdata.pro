!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

TEMPLATE = subdirs

testdata.path = $$DATADIR/testdata
testdata.files = *.wav

INSTALLS *= testdata
