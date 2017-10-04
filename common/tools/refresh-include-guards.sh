#!/bin/bash
for i in */*/*/*.h */*/*/*/*.h; do
    awk  -v "file=$i" -f - "$i" << "-EOF" > "$i".replace
        BEGIN {
            gsub(/[^a-zA-Z0-9_]/, "_", file)
            file = " _EXPORL_" toupper(file) "_"
        }

        / _[A-Z0-9_]+_H_ */ {
            gsub(/ _[A-Z0-9_]+_H_ */, file)
        }

        {
            print
        }
-EOF
done
for i in */*/*/*.h.replace */*/*/*/*.h.replace; do
    j="${i%.replace}"
    if cmp -s "$i" "$j"; then
        rm "$i"
    else
        mv "$i" "$j"
    fi
done
