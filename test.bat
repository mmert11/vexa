@echo off
setlocal enabledelayedexpansion

where cargo >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo Error: cargo is required to build bitwright.
    echo Install Rust via https://rustup.rs/ and try again.
    exit /b 1
)

echo Building tests...
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DVEXA_BUILD_TESTS=ON %*
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

cmake --build build --parallel
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

echo Running tests...
ctest --test-dir build --output-on-failure --parallel
