#!/bin/sh

# Add GPL header to all source and header files

for i in `find ../.. -name '*.cpp' -or -name '*.h'`; do
    sed -i 's/\r//g' $i
    if ! grep License $i >/dev/null ; then
	echo Adding header to $i
        cp $i $i.temp
	cat gplheader.txt $i.temp >$i
        rm $i.temp
    fi
done
