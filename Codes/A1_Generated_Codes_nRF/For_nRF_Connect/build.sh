#!/bin/bash

# Build script for nRF52832 LED and UART Project

echo ""
echo "========================================="
echo "nRF52832 LED and UART Project Builder"
echo "========================================="
echo ""

# Check if west is available
if ! command -v west &> /dev/null; then
    echo "Error: west command not found"
    echo "Please ensure nRF Connect SDK is installed"
    exit 1
fi

# Create build directory
mkdir -p build
cd build

echo "Building project..."

# Configure and build
cmake -DBOARD=nrf52dk_nrf52832 ..
if [ $? -ne 0 ]; then
    echo "Build configuration failed"
    exit 1
fi

cmake --build .
if [ $? -ne 0 ]; then
    echo "Build failed"
    exit 1
fi

cd ..

echo ""
echo "========================================="
echo "Build completed successfully!"
echo "========================================="
echo ""
echo "To flash the firmware, run:"
echo "   west flash"
echo ""
