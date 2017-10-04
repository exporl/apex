clebsCheck(pdflatex) {
    win32 {
        system(pdflatex -version > nul 2>&1):CLEBS_DEPENDENCIES *= pdflatex
    }

    unix {
        system(which pdflatex > /dev/null):CLEBS_DEPENDENCIES *= pdflatex
    }
}

clebsDependency(pdflatex) {
    # Make qmake happy
    CONFIG -= gdb_dwarf_index embed_manifest_exe
    INSTALLS -= target
    win32 {
        QMAKE_LINK = rem
        QMAKE_CXX = rem
        QMAKE_CC = rem
        TEMPBUILDROOT = $$(TEMP)
    }
    unix {
        QMAKE_LINK = true
        QMAKE_CXX = true
        QMAKE_CC = true
        TEMPBUILDROOT = /tmp
    }

    TEMPBUILDLATEX = $${TEMPBUILDROOT}/latex-build$$replace(DESTDIR, [/\\\\:], -)/
    win32:TEMPBUILDLATEX = $$replace(TEMPBUILDLATEX, /, \\\\)
    LATEXOPTIONS = -interaction=nonstopmode -output-directory=$$TEMPBUILDLATEX
    BIBTEXCMD = BIBINPUTS=$$TEMPBUILDLATEX: openout_any=a bibtex

    pdflatex.input = PDFLATEX_SOURCES
    unix:pdflatex.depend_command = ls *.tex
    win32:pdflatex.depend_command = dir /b *.tex
    pdflatex.output = ${QMAKE_FILE_BASE}.pdf
    win32 {
        pdflatex.commands = ( if not exist $${TEMPBUILDLATEX} mkdir $$TEMPBUILDLATEX ) &
        pdflatex.commands += del /q $${TEMPBUILDLATEX}* &
        pdflatex.commands += pdflatex $$LATEXOPTIONS ${QMAKE_FILE_NAME} &
        pdflatex.commands += bibtex $${TEMPBUILDLATEX}${QMAKE_FILE_BASE}.aux &
        pdflatex.commands += pdflatex $$LATEXOPTIONS ${QMAKE_FILE_NAME} &
        pdflatex.commands += pdflatex $$LATEXOPTIONS ${QMAKE_FILE_NAME} &
        pdflatex.commands += $(COPY_FILE) $${TEMPBUILDLATEX}${QMAKE_FILE_BASE}.pdf ${QMAKE_FILE_OUT}
    }
    unix {
        pdflatex.commands = test -d $${TEMPBUILDLATEX} || mkdir -p $$TEMPBUILDLATEX ;
        pdflatex.commands += rm $${TEMPBUILDLATEX}* ;
        pdflatex.commands += test -x pre-build.sh && ./pre-build.sh ;
        pdflatex.commands += pdflatex $$LATEXOPTIONS ${QMAKE_FILE_NAME} ;
        pdflatex.commands += bibtex $${TEMPBUILDLATEX}${QMAKE_FILE_BASE}.aux ;
        pdflatex.commands += pdflatex $$LATEXOPTIONS ${QMAKE_FILE_NAME} ;
        pdflatex.commands += pdflatex $$LATEXOPTIONS ${QMAKE_FILE_NAME} ;
        pdflatex.commands += $(COPY_FILE) $${TEMPBUILDLATEX}${QMAKE_FILE_BASE}.pdf ${QMAKE_FILE_OUT}
    }
    pdflatex.variable_out = GENERATED_FILES
    pdflatex.CONFIG *= target_predeps
    QMAKE_EXTRA_COMPILERS += pdflatex

    }
