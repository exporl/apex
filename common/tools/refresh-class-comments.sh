#!/bin/bash
sed -i.old 's/\(\/\/ .\{46\}======\).*/\1=========================/g' \
    */*/*/*.cpp
for i in */*/*/*.cpp.old; do
    j="${i%.old}"
    if cmp -s "$i" "$j"; then
        mv "$i" "$j"
    else
        rm "$i"
    fi
done
grep -h '======' */*/*/*.cpp
