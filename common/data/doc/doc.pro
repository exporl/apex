!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

TEMPLATE = subdirs

docs.path = $$DOCDIR/common
docs.files = *.md

INSTALLS *= docs
