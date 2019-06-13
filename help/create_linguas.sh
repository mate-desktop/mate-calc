#!/bin/sh

FILE=LINGUAS

if test -f "$FILE"; then
    rm "$FILE";
fi

for file in *;
do
    if [[ -d "$file" && ! -L "$file" ]]; then
        echo "$file" >> LINGUAS;
    fi;
done
