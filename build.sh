#!/bin/bash

# DirPacker Build Script for Linux/macOS

set -e

BUILD_TYPE="${1:-Release}"
BUILD_DIR="build"

echo "======================================"
echo "DirPacker CMake Build Script"
echo "======================================"
echo "Build type: $BUILD_TYPE"
echo ""

# Create build directory
if [ -d "$BUILD_DIR" ]; then
    echo "Cleaning existing build directory..."
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure
echo "Configuring CMake..."
cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -DCMAKE_POLICY_VERSION_MINIMUM=3.5 ..

# Build
echo ""
echo "Building..."
cmake --build . --config "$BUILD_TYPE" -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo ""
echo "======================================"
echo "Build completed successfully!"
echo "======================================"
echo "Executable location: $BUILD_DIR/dirpacker"
echo ""
echo "To run:"
echo "  ./$BUILD_DIR/dirpacker --help"
echo ""
echo "To install (optional):"
echo "  sudo cmake --install $BUILD_DIR"
echo "======================================"
