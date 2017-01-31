#!/bin/bash
sed -i 's/\(\/\/ .\{46\}======\).*/\1=========================/g' \
    src/*/*/*.cpp src/*/*.cpp
grep -h '======' src/*/*/*.cpp src/*/*.cpp
