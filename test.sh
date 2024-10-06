#!/bin/bash
for ((x=1; x<=10; x++))
do
    echo -n "Roll #$x: "
    echo -n $(( $RANDOM % 6 + 1 ))
    echo " " $(( $RANDOM % 6 + 1 ))
done
