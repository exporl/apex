clebsCheck(python) {
    win32 {
        system(python --version 2> NUL):CLEBS_DEPENDENCIES *= python
    }

    unix:!android {
        system(which python 2>&1 > /dev/null):CLEBS_DEPENDENCIES *= python
    }
}

clebsDependency(python) {
    # Make qmake happy
    CONFIG -= gdb_dwarf_index embed_manifest_exe
    INSTALLS -= target
    win32 {
        QMAKE_LINK = rem
        QMAKE_CXX = rem
        QMAKE_CC = rem
    }
    unix {
        QMAKE_LINK = true
        QMAKE_CXX = true
        QMAKE_CC = true
    }

    python.input = PYTHON_SOURCES
    python.output = $$DESTDIR/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
    python.commands = @echo copying python ${QMAKE_FILE_NAME} && $(COPY_FILE) ${QMAKE_FILE_NAME} ${QMAKE_FILE_OUT}
    python.variable_out = GENERATED_FILES
    python.CONFIG += target_predeps
    QMAKE_EXTRA_COMPILERS += python
}
