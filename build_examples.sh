#!/bin/bash

# Script to build all examples
# Usage: ./build_examples.sh [EXAMPLES_DIR]

set -e

if [ -z "$1" ]; then
    echo "Usage: $0 [EXAMPLES_DIR]"
    echo "Error: You must specify the examples directory."
    exit 1
fi
EXAMPLES_DIR="$1"
IDF_PATH=${IDF_PATH:-"$HOME/esp/esp-idf"}

# Source ESP-IDF environment
source "$IDF_PATH/export.sh"

# Set default sdkconfig
cp sdkconfig.ci.defaults.core sdkconfig
idf.py reconfigure

# Find and build all .cpp examples
find "$EXAMPLES_DIR" -name "*.cpp" | while read -r f; do
    echo "Build $f"
    cp "$f" main/main.cpp
    idf.py build || { echo "Error: Build failed for $f"; exit 1; }
done

echo "All examples built successfully!"
