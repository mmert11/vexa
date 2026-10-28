#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

if ! command -v cargo &>/dev/null; then
    echo "Error: cargo is required to build bitwright."
    echo "Install Rust via https://rustup.rs/ and try again."
    exit 1
fi

echo "Building tests..."
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DVEXA_BUILD_TESTS=ON "$@"
cmake --build build -- -j4

echo "Running tests..."
ctest --test-dir build --output-on-failure -j4
