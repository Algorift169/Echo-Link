@echo off
echo Building Echo-Link Server...
if not exist build\server mkdir build\server
where g++ >nul 2>nul
if %errorlevel% equ 0 (
    g++ server/server.cpp -o build/server/server.exe -lws2_32 -std=c++20 -pthread
) else (
    cl server/server.cpp /Fe:build/server/server.exe /std:c++20 ws2_32.lib
)
if %errorlevel% equ 0 (
    echo ✅ Server built successfully!
) else (
    echo ❌ Server build failed!
    exit /b 1
)