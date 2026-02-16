// Cross-platform socket includes
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <netdb.h>
    #include <fcntl.h>
    #include <errno.h>
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
#endif

#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include <cstring>
#include <iomanip>

using namespace std;

// Color codes for terminal output
#ifdef _WIN32
    const string COLOR_RED = "\033[31m";
    const string COLOR_GREEN = "\033[32m";
    const string COLOR_YELLOW = "\033[33m";
    const string COLOR_BLUE = "\033[34m";
    const string COLOR_MAGENTA = "\033[35m";
    const string COLOR_CYAN = "\033[36m";
    const string COLOR_WHITE = "\033[37m";
    const string COLOR_BOLD = "\033[1m";
    const string COLOR_RESET = "\033[0m";
#else
    const string COLOR_RED = "\033[31m";
    const string COLOR_GREEN = "\033[32m";
    const string COLOR_YELLOW = "\033[33m";
    const string COLOR_BLUE = "\033[34m";
    const string COLOR_MAGENTA = "\033[35m";
    const string COLOR_CYAN = "\033[36m";
    const string COLOR_WHITE = "\033[37m";
    const string COLOR_BOLD = "\033[1m";
    const string COLOR_RESET = "\033[0m";
#endif

// Function to display beautiful banner
void displayBanner() {
    cout << COLOR_CYAN << COLOR_BOLD;
    cout << "╔════════════════════════════════════════════════════════════════════════╗\n";
    cout << "║                    ECHO-LINK CHAT SYSTEM                               ║\n";
    cout << "║                                                                        ║\n";
    cout << "║    ███████╗ ██████╗██╗  ██╗ ██████╗     ██╗     ██╗███╗   ██╗██╗  ██╗  ║\n";
    cout << "║    ██╔════╝██╔════╝██║  ██║██╔═══██╗    ██║     ██║████╗  ██║██║ ██╔╝  ║\n";
    cout << "║    █████╗  ██║     ███████║██║   ██║    ██║     ██║██╔██╗ ██║█████╔╝   ║\n";
    cout << "║    ██╔══╝  ██║     ██╔══██║██║   ██║    ██║     ██║██║╚██╗██║██╔═██╗   ║\n";
    cout << "║    ███████╗╚██████╗██║  ██║╚██████╔╝    ███████╗██║██║ ╚████║██║  ██╗  ║\n";
    cout << "║    ╚══════╝ ╚═════╝╚═╝  ╚═╝ ╚═════╝     ╚══════╝╚═╝╚═╝  ╚═══╝╚═╝  ╚═╝  ║\n";
    cout << "║                                                                        ║\n";
    cout << "║                                                                        ║\n";
    cout << "╚════════════════════════════════════════════════════════════════════════╝\n";
    cout << COLOR_RESET << endl;
    
}

// Function to display help menu
void displayHelp() {
    cout << COLOR_YELLOW << COLOR_BOLD;
    cout << "\n📖 Available Commands:\n";
    cout << COLOR_WHITE << "────────────────────────────────────────────────────\n";
    cout << COLOR_GREEN << "  • Type any message" << COLOR_WHITE << "       - Send public message (green)\n";
    cout << COLOR_RED << "  • @username message" << COLOR_WHITE << "      - Send private message (red)\n";
    cout << COLOR_BLUE << "  • /help" << COLOR_WHITE << "                  - Show this help menu\n";
    cout << COLOR_MAGENTA << "  • /clear" << COLOR_WHITE << "                 - Clear screen\n";
    cout << COLOR_CYAN << "  • /users" << COLOR_WHITE << "                 - Show active users\n";
    cout << COLOR_YELLOW << "  • /quit" << COLOR_WHITE << "                  - Exit chat\n";
    cout << "────────────────────────────────────────────────────\n" << COLOR_RESET << endl;
}

// Function to clear screen
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Global variables...
SOCKET clientSocket = INVALID_SOCKET;
string userName;
const int SERVER_PORT = 8080;
const int DISCOVERY_PORT = 8888;

// Text cleaner function
static string trim(const string &s) {
    size_t a = s.find_first_not_of(" \r\n\t");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \r\n\t");
    return s.substr(a, b - a + 1);
}

// Function to receive messages...
void receiveMessages() {
    char buffer[4096];
    while (true) {
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) break;
        buffer[bytesRead] = '\0';
        string message(buffer);

        if (message.rfind("[PRIVATE]", 0) == 0) {
            cout << COLOR_RED << "📨 " << message << " " << COLOR_RESET << endl;
        } else if (message.rfind("[PUBLIC]", 0) == 0) {
            cout << COLOR_GREEN << "💬 " << message << " " << COLOR_RESET << endl;
        } else if (message.rfind("USERS:", 0) == 0) {
            cout << COLOR_CYAN << "\n👥 Online Users: " << message.substr(6) << COLOR_RESET << endl;
        } else if (message.rfind("--- Chat History ---", 0) == 0) {
            cout << COLOR_YELLOW << "\n📜 " << message << COLOR_RESET << endl;
        } else {
            cout << COLOR_WHITE << message << COLOR_RESET << endl;
        }
    }
}

// Auto discover the server...
string findServer(int timeoutMs = 3000) {
    #ifdef _WIN32
        SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    #else
        int udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    #endif
    
    if (udpSocket == INVALID_SOCKET) return "";

    cout << COLOR_YELLOW << "🔍 Searching for server on local network..." << COLOR_RESET << endl;

    int yes = 1;
    setsockopt(udpSocket, SOL_SOCKET, SO_BROADCAST, (char*)&yes, sizeof(yes));

    sockaddr_in broadcastAddr{};
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_port = htons(DISCOVERY_PORT);
    broadcastAddr.sin_addr.s_addr = inet_addr("255.255.255.255");

    const char *discoverMsg = "DISCOVER_CHAT_SERVER";
    sendto(udpSocket, discoverMsg, strlen(discoverMsg), 0, (sockaddr*)&broadcastAddr, sizeof(broadcastAddr));

    // Set timeout
    #ifdef _WIN32
        DWORD t = timeoutMs;
        setsockopt(udpSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&t, sizeof(t));
    #else
        struct timeval tv;
        tv.tv_sec = timeoutMs / 1000;
        tv.tv_usec = (timeoutMs % 1000) * 1000;
        setsockopt(udpSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv));
    #endif

    char buffer[256];
    sockaddr_in serverAddr{};
    socklen_t addrLen = sizeof(serverAddr);

    cout << COLOR_YELLOW << "⏳ Waiting for server response..." << COLOR_RESET << endl;
    
    int len = recvfrom(udpSocket, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&serverAddr, &addrLen);
    string serverIp;
    if (len > 0) {
        buffer[len] = '\0';
        serverIp = buffer;
        cout << COLOR_GREEN << "✅ Server found at: " << serverIp << COLOR_RESET << endl;
    }

    closesocket(udpSocket);
    return serverIp;
}

int main() {
    #ifdef _WIN32
        WSADATA w;
        if (WSAStartup(MAKEWORD(2, 2), &w) != 0) {
            cerr << "WSAStartup failed\n";
            return 1;
        }
    #endif

    // Clear screen and show banner
    clearScreen();
    displayBanner();

    // Server discovery
    string serverIp = findServer(3000);
    if (serverIp.empty()) {
        cout << COLOR_RED << "❌ No server found automatically." << COLOR_RESET << endl;
        cout << COLOR_YELLOW << "Enter server IP (or 127.0.0.1 for local): " << COLOR_RESET;
        getline(cin, serverIp);
        serverIp = trim(serverIp);
        if (serverIp.empty()) serverIp = "127.0.0.1";
    }

    // Connect to server
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Socket creation failed\n";
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(serverIp.c_str());

    cout << COLOR_YELLOW << "🔌 Connecting to server..." << COLOR_RESET << endl;
    
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) != 0) {
        cerr << COLOR_RED << "❌ Could not connect to server\n" << COLOR_RESET;
        return 1;
    }

    cout << COLOR_GREEN << "✅ Connected to server successfully!" << COLOR_RESET << endl;
    cout << COLOR_YELLOW << "\nPlease enter your username to join the chat: " << COLOR_RESET;
    
    getline(cin, userName);
    userName = trim(userName);
    if (userName.empty()) userName = "Guest";
    
    send(clientSocket, userName.c_str(), userName.size(), 0);
    
    cout << COLOR_GREEN << "\n✨ Welcome, " << COLOR_BOLD << userName << COLOR_RESET << COLOR_GREEN << "!\n" << COLOR_RESET;
    
    // Show help menu
    displayHelp();
    
    cout << COLOR_CYAN << "────────────────────────────────────────────────────\n";
    cout << "Start typing your messages below:\n";
    cout << "────────────────────────────────────────────────────\n" << COLOR_RESET;

    // Show chat history if available
    ifstream historyFile("chat_history.txt");
    if (historyFile.is_open()) {
        cout << COLOR_YELLOW << "\n📜 Previous messages:\n";
        string lineFromHistory;
        int lineCount = 0;
        while (getline(historyFile, lineFromHistory) && lineCount < 10) {
            cout << "  " << lineFromHistory << "\n";
            lineCount++;
        }
        if (lineCount == 10) cout << "  ...\n";
        cout << "───────────────────\n" << COLOR_RESET;
    }

    // Start receive thread
    thread(receiveMessages).detach();

    // Main input loop
    string inputLine;
    while (true) {
        cout << COLOR_WHITE << "💭 " << COLOR_RESET;
        getline(cin, inputLine);
        inputLine = trim(inputLine);
        
        if (inputLine.empty()) continue;

        // Handle commands
        if (inputLine == "/help") {
            displayHelp();
            continue;
        }
        else if (inputLine == "/clear") {
            clearScreen();
            displayBanner();
            continue;
        }
        else if (inputLine == "/users") {
            // Request user list from server
            // This will be handled by the server's periodic updates
            continue;
        }
        else if (inputLine == "/quit" || inputLine == "/exit") {
            cout << COLOR_YELLOW << "👋 Goodbye, " << userName << "!\n" << COLOR_RESET;
            break;
        }
        else if (inputLine[0] == '@') {
            size_t spacePos = inputLine.find(' ');
            if (spacePos != string::npos) {
                string targetUser = inputLine.substr(1, spacePos - 1);
                string messageBody = inputLine.substr(spacePos + 1);

                cout << COLOR_RED << "📨 [PRIVATE] " << userName << " -> " << targetUser << ": " << messageBody << COLOR_RESET << endl;

                string outgoingMessage = "/pm " + targetUser + " " + messageBody;
                send(clientSocket, outgoingMessage.c_str(), outgoingMessage.size(), 0);
            } else {
                cout << COLOR_RED << "❌ Invalid private message! Use: @username message\n" << COLOR_RESET;
            }
        }
        else {
            cout << COLOR_GREEN << "💬 [PUBLIC] " << userName << ": " << inputLine << COLOR_RESET << endl;
            send(clientSocket, inputLine.c_str(), inputLine.size(), 0);
        }
    }

    closesocket(clientSocket);
    
    #ifdef _WIN32
        WSACleanup();
    #endif
    
    return 0;
}