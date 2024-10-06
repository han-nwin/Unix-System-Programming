#!/bin/bash
PATH=/bin:/usr/bin
script=$(basename $0)
case $# in
    0)
       vim
       exit 0
       ;;
    1)
       if [ ! -f "$1" ]
           then
              vim "$1"
              exit 0
           fi
       if [ ! -r "$1" -o ! -w "$1" ]
           then
              echo "$script: check permissions on $1" 1>&2
              exit 1
           else
              editfile=$1
            fi
        if [ ! -w "." ]
            then
               echo "$script: backup cannot be created" 1>&2
               exit 1
            fi
        ;;
    *)
        echo "Usage: $script [file-to-edit]" 1>&2
        exit 1
        ;;
esac
tempfile=/tmp/$$.$script
cp $editfile $tempfile
if vim $editfile
    then
        mv $tempfile bak.$(basename $editfile)
        echo "$script: backup file created"
    else
        mv $tempfile editerr
        echo "$script: edit error--copy of original file is in editerr" 1>&2
fi
