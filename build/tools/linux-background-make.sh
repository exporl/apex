#!/bin/bash

# Test for right directory
[ -d src ] || exit 1

GREEN=`tput setaf 2`
RED=`tput setaf 1`
NORMAL=`tput sgr0`

inotifywait  -m  -e close_write data/ contrib/*/ src/*/*/ src/tests/ | while read where what file; do
    extension=${file/#*.}
    [ "$extension" = "ui" -o "$extension" = "pro" -o "$extension" = "cpp" -o "$extension" = "h" ] || continue
    echo ${GREEN}Change detected in $where$file${NORMAL}
    while read -t 3 where what file; do
	echo ${RED}Delaying compile because $where$file also changed${NORMAL}
    done
    LANG=C make 2>&1 | tee .output-background-make
    while [ -f .vim-output-background-make ]; do
	sleep 2
    done
    mv .output-background-make .vim-output-background-make
done

