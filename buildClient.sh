#!/bin/bash
echo "Building Echo-Link Client..."
mkdir -p build/client
g++ client/client.cpp -o build/client/client -std=c++20 -pthread
if [ $? -eq 0 ]; then
    echo "✅ Client built successfully!"
    echo "Location: build/client/client"
else
    echo "❌ Client build failed!"
    exit 1
fi