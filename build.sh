#!/bin/bash
# Build script for NarrativeForge on Linux/macOS
# Requires: CMake, and dependencies

echo "========================================"
echo "NarrativeForge Build Script"
echo "========================================"
echo ""

# Check if we're in the right directory
if [ ! -f "cpp/CMakeLists.txt" ]; then
    echo "ERROR: CMakeLists.txt not found!"
    echo "Please run this script from the project root directory."
    exit 1
fi

echo "Creating build directory..."
mkdir -p cpp/build

cd cpp/build

echo ""
echo "Configuring CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release
if [ $? -ne 0 ]; then
    echo ""
    echo "ERROR: CMake configuration failed!"
    cd ../..
    exit 1
fi

echo ""
echo "Building NarrativeForge..."
make -j$(nproc)
if [ $? -ne 0 ]; then
    echo ""
    echo "ERROR: Build failed!"
    cd ../..
    exit 1
fi

echo ""
echo "========================================"
echo "Build completed successfully!"
echo "========================================"
echo ""
echo "Executables can be found in:"
echo "  cpp/build/"
echo ""
echo "To run the CLI version:"
echo "  ./cpp/build/narrativeforge_cli"
echo ""
echo "To run the GUI version:"
echo "  ./cpp/build/narrativeforge_gui"
echo ""

cd ../..
