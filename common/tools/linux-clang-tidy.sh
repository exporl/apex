#!/bin/bash -e

cd "$(dirname ${BASH_SOURCE[0]})/../.."

rm -r .build
qmake -spec linux-clang
make -j 4 qmake_all
make -j 4
run-clang-tidy-3.8.py
