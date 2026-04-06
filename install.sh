#!/bin/bash

echo "🔧 Installing dependencies..."

# Ubuntu/Debian
if command -v apt-get &> /dev/null; then
    sudo apt-get update
    sudo apt-get install -y build-essential cmake libcurl4-openssl-dev
fi

# macOS
if command -v brew &> /dev/null; then
    brew install cmake curl
fi

echo "✅ Building project..."
mkdir -p build
cd build
cmake ..
make -j4

echo "✅ Done! Run ./build/support_circuit_breaker to start"
