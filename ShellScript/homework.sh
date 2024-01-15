#!/bin/sh

if [ $# -ne 1 ]; then
    echo "Usage: $0 <2-9>"
    exit 1
fi

#if [ $1 -lt 2 ] || [ $1 –gt 9 ]; then
if (( $1 < 2 )) || (( $1 > 9 )); then
    echo "input should be 2~9"
    exit 1
fi

list="1 2 3 4 5 6 7 8 9"

for var in $list
do
    echo "$1 X $var = "`expr $1 \* $var `
done

