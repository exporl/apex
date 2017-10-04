!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

TEMPLATE = subdirs

testdata.path = $$DATADIR/testdata
testdata.files = *.wav *.bin

testblockconfigurations.path = $$DATADIR/testdata/blockconfigurations
testblockconfigurations.files = blockconfigurations/*.txt

INSTALLS *= testdata testblockconfigurations
