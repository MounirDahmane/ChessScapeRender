#!/usr/bin/env bash

set -e

# Default to Debug
BUILD_TYPE=${1:-Debug}
PROJECT_NAME="ChessScapeRender"

if [ ! -d "build" ]; then
    echo "Build directory not found. Running config.sh first..."
    ./config.sh ${BUILD_TYPE}
fi

echo "Building ${PROJECT_NAME}..."

# Build using all available CPU cores (-j)
cmake --build build --parallel $(nproc)

echo "--------------------------------------"
echo "Starting Application..."
echo "--------------------------------------"

# Change directory to build so relative paths (resources/...) work
cd build
./${PROJECT_NAME}