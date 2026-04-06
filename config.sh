#!/usr/bin/env bash

# Exit on error, print commands
set -e
set -x

# Default to Debug if no argument is provided
BUILD_TYPE=${1:-Debug}

if [[ "$BUILD_TYPE" != "Debug" && "$BUILD_TYPE" != "Release" ]]; then
    echo "Usage: $0 [Debug|Release]"
    exit 1
fi

echo "Configuring ${BUILD_TYPE} build with Ninja..."

# -B build: Create/use 'build' directory
# -G Ninja: Use the faster Ninja generator
cmake \
    -B build \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Link compile_commands.json to root so VS Code/Clangd can find it
ln -sf build/compile_commands.json .

echo "Configuration done. Run ./run.sh to build and launch."