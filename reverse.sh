#!/bin/bash

# Check if file.txt exists
if [ -f "file.txt" ]; then
    # Use input redirection to read file.txt with cat
    < file.txt sort
else
    echo "file.txt not found!"
fi

