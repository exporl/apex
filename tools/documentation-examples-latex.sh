#!/bin/bash

MYPATH=`dirname $0`
cd "$MYPATH/../examples"

for path in *; do
        [ -d "${path}" ] || continue
        ls $path/*.apx >/dev/null 2>/dev/null || continue

        echo "\\subsection{$path}"

        for file in $path/*.apx; do
                LATEXFILE=$(echo "$file" |  sed -E 's/([#$%&_\])/\\&/g')
                echo "\\subsubsection{$LATEXFILE}"
                echo '\begin{description}'
                echo "\\item[Short] "
                grep "@what" "$file" | sed "s/.*@what://" | sed -E 's/([#$%&_\])/\\&/g'| sed 's/</\\textless{}/g' | sed 's/>/\\textgreater{}/g'
                echo "\\item[Description] "
                grep "@description" "$file" | sed "s/.*@description://" | sed -E 's/([#$%&_\])/\\&/g'| sed 's/</\\textless{}/g' | sed 's/>/\\textgreater{}/g'
                echo "\\item[How] "
                grep "@how" "$file" | sed "s/.*@how://" | sed -E 's/([#$%&_\])/\\&/g' | sed 's/</\\textless{}/g' | sed 's/>/\\textgreater{}/g'
                echo "\\end{description}"
                echo
        done
done
