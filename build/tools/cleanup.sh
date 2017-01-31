#!/bin/bash

find . -depth \( -name .build -o -name 'Makefile*' -o -name 'object_script.*' \) -exec rm -r {} \;