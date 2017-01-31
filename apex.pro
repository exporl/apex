SUBDIRS = \
    src/apexrunner \
    src/lib/apexdata \
    src/lib/apexmain \
    src/lib/apextools \
    src/lib/apexwriters \
    src/lib/streamapp \
    src/lib/syllib \
    src/lib/psignifit \
#   src/plugins/clarion \      # does not compile any more, should be rewritten
    src/plugins/delayfilter \
    src/plugins/democontroller \
    src/plugins/democontroller \
    src/plugins/dummyfeedbackplugin \
    src/plugins/dummy_slm \
    src/plugins/l34file \
    src/plugins/l34 \
    src/plugins/ledfeedback \
    src/plugins/matlabfilter \
    src/plugins/mwffilter/src/blockdumper \
    src/plugins/mwffilter/src/examples/fadefilter \
#    src/plugins/mwffilter/src/filters/adaptivewienerfilter \
    src/plugins/mwffilter/src/filters/amplifierfilter \
#    src/plugins/mwffilter/src/filters/beamformerfilter \
    src/plugins/mwffilter/src/filters/emphasisfilter \
    src/plugins/mwffilter/src/filters/hrtffilter \
    src/plugins/mwffilter/src/filters/sinkfilter \
    src/plugins/mwffilter/src/filters/threadedhrtffilter \
    src/plugins/mwffilter/src/filters/vadfilter \
#    src/plugins/mwffilter/src/filters/wienerfilter \
    src/plugins/mwffilter/src/hrtf \
    src/plugins/mwffilter/src/original \
    src/plugins/mwffilter/src/profile \
    src/plugins/mwffilter/src/tests \
    src/plugins/scramblespectrumfilter \
    src/plugins/slm_2250 \
    src/plugins/spin \
    src/screeneditor \
    src/testbench/apexdata \
    src/testbench/apexmain \
    src/testbench/apextools \
    src/testbench/apexwriters \
    src/testbench/spin \
    src/testbench/psignifit \
    src/testbench/resultschecker \
    src/testprograms/ledcontrollertest \
    src/testprograms/picturebutton \
    src/testprograms/spin \
    src/testprograms/activeqt-bk2250 \
    data \
    examples \
    matlab \
    doc \
    doc/manual \

CONFIG(FLASH): SUBDIRS += src/testprograms/activeqt-flash

# TODO FIXME whats wrong with those???
# iirfilter
# src/tests/programs/arclayout \

TEMPLATE = subdirs

include (clebs.pri)

doxygen.commands = doxygen doc/technical/Doxyfile > /dev/null
doxytag.commands = doxytag -t doc/qt4.tag /usr/share/qt4/doc/html
todo.commands = @find \\( -name '*.cpp' -o -name '*.h' \\) -exec grep -RHn '\\'TODO\\|FIXME\\|XXX\\|\\todo\\'' {} \\;

win32:test.commands = $$DESTDIR/datatest;$$DESTDIR/maintest;$$DESTDIR/toolstest;$$DESTDIR/writerstest;$$DESTDIR/spintest;
unix:test.commands = $$DESTDIR/datatest|grep -v -i 'QDEBUG';$$DESTDIR/maintest|grep -v -i 'QDEBUG';$$DESTDIR/toolstest|grep -v -i 'QDEBUG';$$DESTDIR/spintest|grep -v -i 'QDEBUG'; $$DESTDIR/syltester

QMAKE_EXTRA_TARGETS *= doxygen doxytag todo test
