@echo off
echo Building Echo-Link Client...
if not exist build\client mkdir build\client
where g++ >nul 2>nul
if %errorlevel% equ 0 (
    g++ client/client.cpp -o build/client/client.exe -lws2_32 -std=c++20 -pthread
) else (
    cl client/client.cpp /Fe:build/client/client.exe /std:c++20 ws2_32.lib
)
if %errorlevel% equ 0 (
    echo ✅ Client built successfully!
) else (
    echo ❌ Client build failed!
    exit /b 1
)