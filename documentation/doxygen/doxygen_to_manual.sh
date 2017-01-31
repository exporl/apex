#!/bin/sh
# Convert doxygen generated latex code to latex suitable for inclusion in apex manual

MYPATH=`dirname $0`
TARGETPATH=$MYPATH/../manual/fromdoxygen
SOURCEPATH=$MYPATH/output/latex

FILES="classapex_1_1_script_procedure_api.tex
classapex_1_1data_1_1_trial.tex
classapex_1_1_screen_result.tex
classapex_1_1data_1_1_script_result_highlight.tex
classapex_1_1_x_m_l_plugin_a_p_i.tex
class_script_procedure_interface.tex
classapex_1_1_html_a_p_i.tex"

for i in $FILES; do
    cat "$SOURCEPATH/$i" | sed 's/\\subsubsection/\\paragraph/' | sed 's/\\subsection/\\subsubsection/' | sed 's/\\section{.*}//'  >"$TARGETPATH/$i"
done
cp $SOURCEPATH/doxygen.sty $TARGETPATH/..
