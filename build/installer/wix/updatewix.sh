#!/bin/bash

TARGET="examplesfiles.wxi"

cp "wixheader.xml" $TARGET
./generateexamples.pl >>$TARGET
cat "wixfooter.xml">>$TARGET
