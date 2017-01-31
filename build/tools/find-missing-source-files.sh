#!/bin/bash

# Test for right directory
[ -d src ] || exit 1

for i in src; do
    for j in apex; do
	if [ ! -d $i/$j ]; then
	    continue
	fi
	diff -u <(eval ls $i/$j/\{*,*/*,*/*/*}.\{h,cpp} | grep -v 'pch\.h' | sort) \
	    <(grep '\.h\|\.cpp' $i/$j/$j.pro | grep -v 'pch\.h' | \
		sed 's|\s\+\\\s*$||;s|^\s\+|'"$i"'/'"$j"'/|' | sort)
    done
done
