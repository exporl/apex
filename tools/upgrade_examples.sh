#!/bin/bash

SOURCE=~/data/apex/examples
TARGET=~/temp/examples_converted
CONVERTER=~/data/apex/bin/debug/experimentupgrader

cd $SOURCE
# Create target folders
for f in `find -type d`; do
    mkdir $TARGET/$f
done

for f in `find -name "*.apx" -or -name "*.xml"`; do
    $CONVERTER -f "$SOURCE/$f" "$TARGET/$f"
done
