#!/bin/bash

for f in examples/*.cpp
do
    echo "********************"
    echo "build $f"
    echo "********************"
    cat $f > main/main.cpp
    pio run -e core
done
