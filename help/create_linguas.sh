#!/bin/sh

FILE=LINGUAS

if test -f "$FILE"; then
    rm "$FILE";
fi

for dir in *;
do
    if [[ -d "$dir" && ! -L "$dir" && "$dir" != "C" ]]; then
        echo "$dir" >> LINGUAS;
    fi;
done
