CLEBS *= pdflatex

!isEmpty(_PRO_FILE_):include(../../clebs/clebs.pri)

manual.files = apexpaper.pdf referencemanual.pdf apex3manual.pdf
manual.path = $$DOCDIR
INSTALLS *= manual

PDFLATEX_SOURCES *= apex3manual.tex
