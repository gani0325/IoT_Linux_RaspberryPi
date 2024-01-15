#!/bin/sh

if [ $# -ne 1 ]; then
    echo "Usage: $0 <directory name>"
    exit 1
fi

find $1 -empty -type d -exec touch {}/.gitkeep \;
