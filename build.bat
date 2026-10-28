@echo off
setlocal enabledelayedexpansion

where cargo >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo Error: cargo is required to build bitwright.
    echo Install Rust via https://rustup.rs/ and try again.
    exit /b 1
)

if exist .gitmodules (
    where git >nul 2>&1
    if %ERRORLEVEL% equ 0 (
        if not exist external\remill\CMakeLists.txt (
            echo Updating git submodules...
            git submodule update --init --recursive
        )
    )
)

echo Building vexa and vexa-cli...
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DVEXA_BUILD_TESTS=OFF %*
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

cmake --build build --parallel
