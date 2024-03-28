#!/bin/bash

for f in examples/*.cpp
do
    echo "Build $f"
    cat $f > main/main.cpp
    pio run -e core || { echo "Error: Build failed for $f"; exit 1; }
done
