#!/bin/bash

echo 'Without quotes:'
echo $*

echo $@

echo
echo 'With quotes:'
echo "$*"

echo "$@"

