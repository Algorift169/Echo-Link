Echo-Link: A Local Network Chat Application

A cross-platform chat application with automatic server discovery, public/private messaging, and real-time user lists. Messages are color-coded: public in GREEN, private in RED.
✨ Features

    Automatic Server Discovery: Clients automatically find the server on the same local network using UDP broadcast

    Username-based Login: Each user connects with a unique username (duplicates are auto-renamed)

    Public Chat: Broadcast messages to all connected users (displayed in GREEN)

    Private Chat: Send direct messages using @username (displayed in RED)

    Real-time User List: Displays all active participants (updates every 2 seconds)

    Chat History: Messages are saved and loaded when reconnecting

    Cross-Platform: Works on both Windows and Linux

📁 Project Structure

Echo-Link/
├── client/
│   ├── client.cpp         # Client application
│   └── CMakeLists.txt      # Client build configuration
├── server/
│   ├── server.cpp         # Server application
│   └── CMakeLists.txt      # Server build configuration
├── build/                  # Build output directory (created automatically)
│   ├── server/            # Server executables
│   └── client/            # Client executables
├── buildServer.sh          # Linux server build script
├── buildServer.bat         # Windows server build script
├── buildClient.sh          # Linux client build script
├── buildClient.bat         # Windows client build script
├── linrequirements.txt     # Linux requirements (apt installable)
├── linrequirements.sh      # Linux automatic install script
├── winrequirements.txt     # Windows requirements (chocolatey)
├── winrequirements.ps1     # Windows automatic install script
└── README.md              # Project documentation

🛠️ Requirements
Linux (Kali/Ubuntu/Debian)

    Linux kernel 3.2 or newer

    GCC 8.1.0 or newer (with C++20 support)

    pthread library

    CMake 3.31 or newer (optional)

Windows

    Windows 7 or newer

    C++17 or later compiler (MinGW-w64 or MSVC)

    Winsock2 library (included with Windows SDK)

    CMake 3.31 or newer (optional)

    Chocolatey (for automatic installation)

🚀 Installation
Linux Installation
Method 1: Using requirements file (recommended)

# Navigate to project root
cd /path/to/Echo-Link

# Install all requirements
sudo apt-get update
sudo apt-get install -y $(cat linrequirements.txt)

# Make scripts executable
chmod +x buildServer.sh buildClient.sh linrequirements.sh

# Run the automatic install script
./linrequirements.sh

_____________________________________________________________________

Windows Installation
Method 1: Using Chocolatey (recommended)

# Open PowerShell as Administrator
# Navigate to project root
cd C:\path\to\Echo-Link

# Install from requirements file
Get-Content winrequirements.txt | ForEach-Object { choco install -y $_ }

Method 2: Using PowerShell script

# Run as Administrator
.\winrequirements.ps1

Method 3: Manual Installation

    Download and install MinGW-w64

    Download and install CMake

    Add both to system PATH

===============================================
🔨 Building the Application
On Linux
------------------------------------------------
# Build the server
./buildServer.sh

# Build the client
./buildClient.sh

================================================
On Windows

:: Build the server
buildServer.bat

:: Build the client
buildClient.bat

=================================================
Manual Build (if scripts don't work)
Linux:
-------------------------------------------------

# Build server
cd server
g++ server.cpp -o ../build/server/server -std=c++20 -pthread

# Build client
cd client
g++ client.cpp -o ../build/client/client -std=c++20 -pthread

Windows (MinGW):

:: Build server
cd server
g++ server.cpp -o ..\build\server\server.exe -lws2_32 -std=c++20 -pthread

:: Build client
cd client
g++ client.cpp -o ..\build\client\client.exe -lws2_32 -std=c++20 -pthread

Windows (MSVC):

:: Build server
cd server
cl server.cpp /Fe:..\build\server\server.exe /std:c++20 ws2_32.lib

:: Build client
cd client
cl client.cpp /Fe:..\build\client\client.exe /std:c++20 ws2_32.lib

===========================================================
🎮 Running the Application
Start the Server First

# Linux
./build/server/server

# Windows
.\build\server\server.exe

Start Client(s) (in separate terminals)

# Linux
./build/client/client

# Windows
.\build\client\client.exe

______________________________________________________________________
-----------------------------------------------------------------------
📝 Usage Guide

    Start the Server

        Run the server executable

        It will display its IP address and wait for connections

    Start a Client

        Run the client executable

        Client automatically discovers server on local network

        If discovery fails, enter server IP manually

    Enter Username

        Type your desired username when prompted

        If username exists, you'll get a unique variant (e.g., "John" → "John_1")

    Chat Commands

        Public Message: Just type your message and press Enter (appears in GREEN)

        Private Message: Use @username your message (appears in RED)
        Example: @john Hello John, how are you?

    View Active Users

        User list updates automatically every 2 seconds

        Shows all connected usernames

🌐 Network Details

    Server Port (TCP): 8080 - Main chat communication

    Discovery Port (UDP): 8888 - Server discovery broadcasts

    Protocol: Messages are sent as plain text

    Discovery: Clients broadcast "DISCOVER_CHAT_SERVER", server responds with its IP

    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    =======================================

    ⚠️ Important Notes

    Server must be running BEFORE any clients connect

    Works only on local networks (LAN)

    Multiple clients can connect simultaneously

    Duplicate usernames are automatically renamed with suffixes

    Firewall must allow ports 8080 (TCP) and 8888 (UDP)

    Chat history is saved in chat_history.txt in the server directory

🔥 Firewall Configuration
Linux (UFW)
bash

sudo ufw allow 8080/tcp
sudo ufw allow 8888/udp
sudo ufw reload

Windows (PowerShell as Admin)
powershell

New-NetFirewallRule -DisplayName "Echo-Link TCP" -Direction Inbound -Protocol TCP -LocalPort 8080 -Action Allow
New-NetFirewallRule -DisplayName "Echo-Link UDP" -Direction Inbound -Protocol UDP -LocalPort 8888 -Action Allow

🐛 Troubleshooting
Linux Issues
bash

# Permission denied for scripts
chmod +x buildServer.sh buildClient.sh

# Port already in use
sudo lsof -i :8080
sudo kill -9 <PID>

# Compiler not found
sudo apt-get install build-essential

Windows Issues
cmd

:: "g++ not recognized"
:: Add MinGW to PATH or reinstall MinGW

:: Port 8080 in use
netstat -ano | findstr :8080
taskkill /PID <PID> /F

:: Build fails
:: Run Command Prompt as Administrator and try again

📄 License

This project is open source and available for educational purposes.
🤝 Contributing

Feel free to fork this project and submit pull requests for improvements!
📧 Contact

For issues or questions, please open an issue on the project repository.

Happy Chatting! 🎉