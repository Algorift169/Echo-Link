Echo-Link: A Local Network Chat Application

✨ Features:

Automatic server discovery on the same local network.
Username-based login for better identification.
Public chat: Broadcast messages to all connected users.
Private chat: Send direct messages using @username.
Real-time user list: Displays all active participants.
Chat history: Messages are saved for reference after reconnecting.

🛠️ Requirements:

Windows OS

C++17 or later

Winsock2 library (already included with Windows SDK)

CMake (for building with CLion or other IDEs)


Project Structure:

├── server.cpp         # Server application

├── client.cpp         # Client application

├── CMakeLists.txt     # Build configuration

├── chat_history.txt   # Stored messages

└── README.md          # Project documentation


Notes:

Works only on local networks (LAN).
Multiple clients can connect to the same server.
Duplicate usernames are automatically renamed.
