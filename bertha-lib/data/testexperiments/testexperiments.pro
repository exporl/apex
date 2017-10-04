!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

TEMPLATE = subdirs

testexperiments.path = $$DATADIR/testexperiments
testexperiments.files = *.xml

INSTALLS *= testexperiments
