#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

if ! command -v cargo &>/dev/null; then
    echo "Error: cargo is required to build bitwright."
    echo "Install Rust via https://rustup.rs/ and try again."
    exit 1
fi

if [ -f ".gitmodules" ] && command -v git &>/dev/null; then
    if [ ! -f "external/remill/CMakeLists.txt" ] || [ ! -f "external/bitwright/Cargo.toml" ] || [ ! -f "external/lief/CMakeLists.txt" ] || [ ! -f "external/quill/CMakeLists.txt" ] || [ ! -f "external/cli11/CMakeLists.txt" ]; then
        echo "Updating git submodules..."
        git submodule update --init --recursive
    fi
fi

if [ ! -f "external/remill/install/lib/libremill_arch.a" ] && [ ! -f "external/remill/install/lib/remill_arch.lib" ]; then
    echo "Building remill..."
    cmake -G Ninja -S external/remill/dependencies -B external/remill/dependencies/build \
        -DUSE_EXTERNAL_LLVM=ON \
        -DCMAKE_INSTALL_PREFIX="$SCRIPT_DIR/external/remill/install" \
        -DCMAKE_BUILD_TYPE=Release
    cmake --build external/remill/dependencies/build -- -j4

    cmake -G Ninja -S external/remill -B external/remill/build \
        -DCMAKE_PREFIX_PATH="$SCRIPT_DIR/external/remill/install" \
        -DCMAKE_INSTALL_PREFIX="$SCRIPT_DIR/external/remill/install" \
        -DCMAKE_BUILD_TYPE=Release
    cmake --build external/remill/build --target install -- -j4
fi

echo "Building vexa and vexa-cli..."
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DVEXA_BUILD_TESTS=OFF "$@"
cmake --build build -- -j4
