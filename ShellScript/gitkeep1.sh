#!/bin/sh
iterate_dir()
{
    filelist=`ls -A $1`
    for file in $filelist
    do
        filelist=`ls -A $1/$file`
        if [ "$filelist" = "" ]; then
            echo "Create $1/$file/.gitkeep"
            touch $1/$file/.gitkeep
        elif [ -d $1/$file ]; then
            iterate_dir $1/$file
        fi
    done
}

if [ $# -ne 1 ]; then
    echo "Usage: $0 <directory name>"
    exit 1
fi

iterate_dir $1

