#!/bin/bash
echo "Building Echo-Link Server..."
mkdir -p build/server
g++ server/server.cpp -o build/server/server -std=c++20 -pthread
if [ $? -eq 0 ]; then
    echo "✅ Server built successfully!"
    echo "Location: build/server/server"
else
    echo "❌ Server build failed!"
    exit 1
fi