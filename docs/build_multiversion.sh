#!/bin/bash
# Script to build documentation with versioning support
# Usage: ./build_multiversion.sh [tag]

# If a tag is provided, build to build/html/<tag>/
# Otherwise, build to build/html/latest/

TAG="$1"

if [ -z "$TAG" ]; then
    # Normal build to latest
    OUTPUT_DIR="build/html/latest"
    VERSION_NAME="latest"
    echo "Building documentation -> $OUTPUT_DIR/"
else
    # Build for a specific tag
    OUTPUT_DIR="build/html/$TAG"
    VERSION_NAME="$TAG"
    echo "Building documentation for tag $TAG -> $OUTPUT_DIR/"
fi

# Export version for conf.py to read
export DOCS_VERSION="$VERSION_NAME"

# Build documentation with current version in context
sphinx-build \
    -b html \
    source \
    "$OUTPUT_DIR"

cp index.html build/html/index.html

echo "Documentation built in $OUTPUT_DIR/"
