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

using namespace std;

// Color codes for terminal output
#ifdef _WIN32
    // Windows doesn't support ANSI colors by default in older versions
    // But we'll keep them for compatibility with Windows 10+
    const string COLOR_RED = "\033[31m";
    const string COLOR_GREEN = "\033[32m";
    const string COLOR_RESET = "\033[0m";
#else
    const string COLOR_RED = "\033[31m";
    const string COLOR_GREEN = "\033[32m";
    const string COLOR_RESET = "\033[0m";
#endif

// Global variables...
SOCKET clientSocket = INVALID_SOCKET;
string userName;
const int SERVER_PORT = 8080;             // Main server port (TCP)
const int DISCOVERY_PORT = 8888;         // Server discovery port (UDP)

// Text cleaner function: To remove carriage returns, newlines & tabs...
static string trim(const string &s) {
    size_t a = s.find_first_not_of(" \r\n\t");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \r\n\t");
    return s.substr(a, b - a + 1);
}

// Function to receive messages...
void receiveMessages() {
    char buffer[4096];            // MSG Space = 4 KB
    while (true) {
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) break;
        buffer[bytesRead] = '\0';
        string message(buffer);

        if (message.rfind("[PRIVATE]", 0) == 0) {
            // Private messages in RED
            cout << COLOR_RED << "<<- " << message << " " << COLOR_RESET << endl;
        } else if (message.rfind("[PUBLIC]", 0) == 0) {
            // Public messages in GREEN
            cout << COLOR_GREEN << "<<- " << message << " " << COLOR_RESET << endl;
        } else if (message.rfind("USERS:", 0) == 0) {
            cout << "\n--- Users Online --- " << message.substr(6) << " ---\n";
        } else {
            cout << message << endl;
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

    int len = recvfrom(udpSocket, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&serverAddr, &addrLen);
    string serverIp;
    if (len > 0) {
        buffer[len] = '\0';
        serverIp = buffer;
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

    cout << "Searching for server on local network...\n";
    string serverIp = findServer(3000);
    if (serverIp.empty()) {
        cout << "No server found automatically. Enter server IP (or 127.0.0.1): ";
        getline(cin, serverIp);
        serverIp = trim(serverIp);
        if (serverIp.empty()) serverIp = "127.0.0.1";
    } else {
        cout << "Server found at: " << serverIp << "\n";
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "socket failed\n";
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(serverIp.c_str());

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) != 0) {
        cerr << "Could not connect to server\n";
        return 1;
    }

    cout << "Please enter your username to join the chat: ";
    getline(cin, userName);
    userName = trim(userName);
    if (userName.empty()) userName = "NoName";
    send(clientSocket, userName.c_str(), userName.size(), 0);

    ifstream historyFile("chat_history.txt");
    if (historyFile.is_open()) {
        cout << "--- Old Messages ---\n";
        string lineFromHistory;
        while (getline(historyFile, lineFromHistory)) cout << lineFromHistory << "\n";
        cout << "-------------------\n";
    }

    thread(receiveMessages).detach();

    string inputLine;
    while (true) {
        getline(cin, inputLine);
        inputLine = trim(inputLine);
        if (inputLine.empty()) continue;

        if (inputLine[0] == '@') {
            size_t spacePos = inputLine.find(' ');
            if (spacePos != string::npos) {
                string targetUser = inputLine.substr(1, spacePos - 1);
                string messageBody = inputLine.substr(spacePos + 1);

                // Show your own private message in RED
                cout << COLOR_RED << "[PRIVATE] " << userName << ": " << messageBody << " " << COLOR_RESET << endl;

                string outgoingMessage = "/pm " + targetUser + " " + messageBody;
                send(clientSocket, outgoingMessage.c_str(), outgoingMessage.size(), 0);
                continue;
            } else {
                cout << "[CLIENT] Invalid private format! Please Use: @username(space)message\n";
                continue;
            }
        }

        // Show your own public message in GREEN
        cout << COLOR_GREEN << "[PUBLIC] " << userName << ": " << inputLine << " " << COLOR_RESET << endl;
        send(clientSocket, inputLine.c_str(), inputLine.size(), 0);
    }

    closesocket(clientSocket);
    
    #ifdef _WIN32
        WSACleanup();
    #endif
    
    return 0;
}