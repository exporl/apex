!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

TEMPLATE = subdirs

schemas.path = $$DATADIR/schema
schemas.files = *.xsd

INSTALLS *= schemas
