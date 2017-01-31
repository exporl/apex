clebsCheck(pdflatex) {              
    win32:system(pdflatex -version > nul 2>&1):CLEBS_DEPENDENCIES *= pdflatex
    unix:system(which pdflatex > /dev/null):CLEBS_DEPENDENCIES *= pdflatex
}                                                                                                                               
                                                                                                                                    
clebsDependency(pdflatex) {                                                                                                          
    pdflatex.output = ${QMAKE_FILE_BASE}.pdf
    pdflatex.clean = ${QMAKE_FILE_BASE}.idx ${QMAKE_FILE_BASE}.pdf \
		     ${QMAKE_FILE_BASE}.bbl ${QMAKE_FILE_BASE}.blg \
		     ${QMAKE_FILE_BASE}.aux ${QMAKE_FILE_BASE}.dvi \
		     ${QMAKE_FILE_BASE}.ilg ${QMAKE_FILE_BASE}.ind \
		     ${QMAKE_FILE_BASE}.log ${QMAKE_FILE_BASE}.toc
    pdflatex.input = PDFLATEX_SOURCE
    pdflatex.commands = pdflatex ${QMAKE_FILE_IN} && pdflatex ${QMAKE_FILE_IN} && pdflatex ${QMAKE_FILE_IN}
    pdflatex.CONFIG = no_link
		 
    pdflatex.variable_out = PDFLATEX_OUTPUT
    pdflatex.name = pdflatex

    CONFIG -= embed_manifest_exe		 # don't make manifest for pdf files
    QMAKE_EXTRA_COMPILERS += pdflatex
}
