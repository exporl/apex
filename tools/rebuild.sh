#!/bin/bash

if [ "`uname -o`" == "Cygwin" ]; then
    MAKE=nmake
else
    MAKE="make -j 4"
fi

find . -depth \( -name .build -o -name Makefile \) -exec rm -r {} \;
rm -r bin .build
qmake
$MAKE
