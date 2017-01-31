#!/bin/bash
for i in src/*/*.h src/*/*/*.h; do
    awk  -v "file=$i" -f - "$i" << "-EOF" > "$i".replace
	BEGIN {
	    "pwd" | getline pwd
	    gsub (/.*\//, "", pwd)
	    gsub (/\W/, "_", file)
	    file = "_" toupper (pwd) "_" toupper (file) "_"
	}

	/_\w+_H_/ {
	    gsub (/_\w+_H_/, file)
	}

	{
	    print
	}
-EOF
done
for i in src/*/*.h.replace src/*/*/*.h.replace; do
    j=${i%.replace}
    mv $i $j
done
