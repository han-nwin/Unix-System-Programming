#!/bin/bash

if [ $# -eq 0 ]
then
    echo "Usage: whos id..." 1>&2
    exit 1
fi

for id
do
    gawk -F: '{print $1, $5}' /etc/passwd | grep -i "$id"
done
# Use of "$@"
# The whos script uses a for statement to loop through the command-line arguments. 
# In this script the implied use of "$@" in the for loop is particularly beneficial 
# because it causes the for loop to treat an argument that contains a SPACE 
# as a single argument.
