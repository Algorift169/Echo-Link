# winrequirements.ps1 - Automatic installer for Windows requirements using Chocolatey

Write-Host "===================================" -ForegroundColor Cyan
Write-Host "Echo-Link Windows Requirements Installer" -ForegroundColor Cyan
Write-Host "===================================" -ForegroundColor Cyan
Write-Host ""

# Check if running as Administrator
$isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) {
    Write-Host "❌ This script must be run as Administrator!" -ForegroundColor Red
    Write-Host "Please right-click PowerShell and select 'Run as Administrator'" -ForegroundColor Yellow
    exit 1
}

# Check if Chocolatey is installed
if (-not (Get-Command choco -ErrorAction SilentlyContinue)) {
    Write-Host "Installing Chocolatey..." -ForegroundColor Yellow
    Set-ExecutionPolicy Bypass -Scope Process -Force
    [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
    iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
    
    # Refresh environment
    $env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")
}

Write-Host "Installing required packages..." -ForegroundColor Yellow

# Install CMake
Write-Host "Installing CMake..." -ForegroundColor White
choco install cmake --installargs 'ADD_CMAKE_TO_PATH=System' -y

# Install MinGW
Write-Host "Installing MinGW..." -ForegroundColor White
choco install mingw -y

# Install Git (optional)
Write-Host "Installing Git..." -ForegroundColor White
choco install git -y

Write-Host ""
Write-Host "Checking installations..." -ForegroundColor Cyan
Write-Host "------------------------"

# Check GCC
if (Get-Command g++ -ErrorAction SilentlyContinue) {
    $gccVersion = & g++ --version | Select-Object -First 1
    Write-Host "✅ GCC installed: $gccVersion" -ForegroundColor Green
} else {
    Write-Host "❌ GCC not found in PATH" -ForegroundColor Red
}

# Check CMake
if (Get-Command cmake -ErrorAction SilentlyContinue) {
    $cmakeVersion = & cmake --version | Select-Object -First 1
    Write-Host "✅ CMake installed: $cmakeVersion" -ForegroundColor Green
} else {
    Write-Host "❌ CMake not found in PATH" -ForegroundColor Red
}

Write-Host ""
Write-Host "===================================" -ForegroundColor Cyan
Write-Host "✅ All requirements installed successfully!" -ForegroundColor Green
Write-Host "===================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Next steps:"
Write-Host "1. Build server: .\buildServer.bat"
Write-Host "2. Build client: .\buildClient.bat"
Write-Host "3. Run server: .\build\server\server.exe"
Write-Host "4. Run client: .\build\client\client.exe"