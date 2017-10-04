!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

TEMPLATE = subdirs

regression.path = $$DATADIR/cohregressiontests
regression.files = *.aseq *.m *.txt *.xml

INSTALLS *= regression
