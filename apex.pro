TEMPLATE = subdirs
CLEBS_TREES = src bertha-lib mwffilter apex-confidential common
!android:CLEBS_TREES *= coh
android:CLEBS_DISABLED *= src/plugins/protoslmplugin
SUBDIRS = data documentation/manual examples matlab toolbox/matlab

# remove them from CLEBS_DISABLED in localconfig.pri if you want to build them automatically
CLEBS_DISABLED *= documentation/manual src/data/nicinstall

!isEmpty(_PRO_FILE_):include(clebs/clebs.pri)

doxygen.commands = doxygen documentation/doxygen/Doxyfile > /dev/null; sh documentation/doxygen/doxygen_to_manual.sh
doxytag.commands = doxytag -t doc/qt4.tag /usr/share/qt4/doc/html
todo.commands = @find \\( -name '*.cpp' -o -name '*.h' \\) -exec grep -RHn '\\'TODO\\|FIXME\\|XXX\\|\\todo\\'' {} \\;

#unix:bigtest.commands = $$BASEDIR/tools/bigtest.sh;

QMAKE_EXTRA_TARGETS *= doxygen doxytag todo
