#!/bin/bash
# linrequirements.sh - Automatic installer for Linux requirements

echo "==================================="
echo "Echo-Link Linux Requirements Installer"
echo "==================================="
echo ""

echo "Updating package list..."
sudo apt-get update

echo ""
echo "Installing required packages..."
sudo apt-get install -y \
    build-essential \
    cmake \
    g++ \
    make \
    git

echo ""
echo "Checking installations..."
echo "------------------------"

# Check GCC
if command -v g++ &> /dev/null; then
    echo "✅ GCC installed: $(g++ --version | head -n1)"
else
    echo "❌ GCC installation failed"
fi

# Check CMake
if command -v cmake &> /dev/null; then
    echo "✅ CMake installed: $(cmake --version | head -n1)"
else
    echo "❌ CMake installation failed"
fi

# Check Make
if command -v make &> /dev/null; then
    echo "✅ Make installed: $(make --version | head -n1)"
else
    echo "❌ Make installation failed"
fi

echo ""
echo "==================================="
echo "✅ All requirements installed successfully!"
echo "==================================="
echo ""
echo "Next steps:"
echo "1. Build server: ./buildServer.sh"
echo "2. Build client: ./buildClient.sh"
echo "3. Run server: ./build/server/server"
echo "4. Run client: ./build/client/client"