#!/bin/bash
for var in `grep -l '.\{81,\}' src/*/*.{h,cpp} src/*/*/*.{h,cpp}`; do
    vi -c '/.\{81\}' $var
done

