// This is the server....
#include <iostream>
#include <winsock2.h>
#include <thread>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <mutex>
#include <unordered_map>
#include <chrono>

#pragma comment(lib,"ws2_32.lib")
using namespace std;

// Structure to store information about a client
struct Client {
    SOCKET clientSocket;
    string userName;
};

// Global variables for client management
mutex clientMutex;
vector<Client> clients;
unordered_map<string, SOCKET> userSocketMap; // maps username -> client socket

// Helper function to trim spaces and newlines from a string
static string trim(const string &input) {
    size_t start = input.find_first_not_of(" \r\n\t");
    if (start == string::npos) return "";
    size_t end = input.find_last_not_of(" \r\n\t");
    return input.substr(start, end-start+1);
}

// Send user list to all clients without locking (assumes caller has locked)
void sendUserListUnsafe() {
    string usersMessage = "USERS:";
    for (auto &client : clients) {
        usersMessage += client.userName + ",";
    }
    for (auto &client : clients) {
        send(client.clientSocket, usersMessage.c_str(), (int)usersMessage.size(), 0);
    }
}

// Send user list to all clients
void sendUserList() {
    lock_guard<mutex> lock(clientMutex);
    sendUserListUnsafe();
}

// Send a message to a single client
void sendMessage(SOCKET clientSocket, const string &message) {
    send(clientSocket, message.c_str(), (int)message.size(), 0);
}

// Broadcast a public message to all clients except the sender
void sendPublicMessage(const string &formattedMessage, SOCKET senderSocket) {
    lock_guard<mutex> lock(clientMutex);

    cout << formattedMessage << endl; // server log

    // Save to chat history
    ofstream historyFile("chat_history.txt", ios::app);
    if (historyFile.is_open()) historyFile << formattedMessage << "\n";

    for (auto &client : clients) {
        if (client.clientSocket != senderSocket) {
            sendMessage(client.clientSocket, formattedMessage);
        }
    }
}

// Send private message to a specific target user
void sendPrivateMessage(const string &formattedMessage, const string &targetUser, SOCKET senderSocket) {
    lock_guard<mutex> lock(clientMutex);

    cout << formattedMessage << endl; // server log

    // Save to chat history
    ofstream historyFile("chat_history.txt", ios::app);
    if (historyFile.is_open()) historyFile << formattedMessage << "\n";

    auto it = userSocketMap.find(targetUser);
    if (it != userSocketMap.end()) {
        SOCKET targetSocket = it->second;
        sendMessage(targetSocket, formattedMessage); // deliver to target only
    } else {
        // notify sender if user not found
        for (auto &client : clients) {
            if (client.clientSocket == senderSocket) {
                string serverReply = "[SERVER] User '" + targetUser + "' not found.";
                sendMessage(senderSocket, serverReply);
                break;
            }
        }
    }
}

// Remove a client from the lists when disconnected
void removeClient(SOCKET clientSocket) {
    lock_guard<mutex> lock(clientMutex);

    string nameToErase;
    for (auto &client : clients) {
        if (client.clientSocket == clientSocket) {
            nameToErase = client.userName;
            break;
        }
    }

    if (!nameToErase.empty()) userSocketMap.erase(nameToErase);

    clients.erase(remove_if(clients.begin(), clients.end(),
        [&](const Client &c){ return c.clientSocket == clientSocket; }), clients.end());
}

// Function to handle all messages from a single client
void clientHandler(Client clientInfo) {
    char buffer[4096];
    SOCKET clientSocket = clientInfo.clientSocket;

    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer)-1, 0);
        if (bytesReceived <= 0) break;

        buffer[bytesReceived] = '\0';
        string message = trim(string(buffer));
        if (message.empty()) continue;

        // Check if message is private: "/pm username message"
        if (message.rfind("/pm ", 0) == 0) {
            size_t spacePos = message.find(' ', 4);
            if (spacePos != string::npos) {
                string targetUser = trim(message.substr(4, spacePos - 4));
                string messageBody = message.substr(spacePos + 1);
                string formattedMessage = "[PRIVATE] " + clientInfo.userName + ": " + messageBody;
                sendPrivateMessage(formattedMessage, targetUser, clientSocket);
            } else {
                string serverReply = "[SERVER] Invalid /pm format. Use: /pm username message";
                sendMessage(clientSocket, serverReply);
            }
        } else {
            // Public message
            string formattedMessage = "[PUBLIC] " + clientInfo.userName + ": " + message;
            sendPublicMessage(formattedMessage, clientSocket);
        }
    }

    // Client disconnected
    closesocket(clientSocket);
    removeClient(clientSocket);
    sendUserList();
    cout << "[SERVER] User disconnected: " << clientInfo.userName << endl;
}

// Get local IP address of the machine
string getLocalIp() {
    char hostName[256];
    if (gethostname(hostName, sizeof(hostName)) == SOCKET_ERROR) return "127.0.0.1";
    hostent* hostInfo = gethostbyname(hostName);
    if (!hostInfo) return "127.0.0.1";

    for (int i=0; hostInfo->h_addr_list[i]; ++i) {
        in_addr addr;
        memcpy(&addr, hostInfo->h_addr_list[i], sizeof(in_addr));
        string ip = inet_ntoa(addr);
        if (ip != "127.0.0.1") return ip;
    }

    return inet_ntoa(*(in_addr*)hostInfo->h_addr_list[0]);
}

// UDP thread to respond to server discovery messages
void udpDiscoveryResponder(unsigned short port) {
    SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocket == INVALID_SOCKET) return;

    sockaddr_in udpAddr{};
    udpAddr.sin_family = AF_INET;
    udpAddr.sin_port = htons(port);
    udpAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(udpSocket, (sockaddr*)&udpAddr, sizeof(udpAddr)) == SOCKET_ERROR) {
        closesocket(udpSocket);
        return;
    }

    char buffer[512];
    sockaddr_in clientAddr{};
    int clientAddrLen = sizeof(clientAddr);
    string localIp = getLocalIp();

    while (true) {
        int len = recvfrom(udpSocket, buffer, sizeof(buffer)-1, 0, (sockaddr*)&clientAddr, &clientAddrLen);
        if (len > 0) {
            buffer[len] = '\0';
            string received = buffer;
            if (received == "DISCOVER_CHAT_SERVER") {
                sendto(udpSocket, localIp.c_str(), (int)localIp.size(), 0, (sockaddr*)&clientAddr, clientAddrLen);
            }
        }
    }
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        cerr << "WSAStartup failed\n";
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Socket creation failed\n";
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Bind failed\n";
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "Listen failed\n";
        return 1;
    }

    cout << "[SERVER] Listening on port 8080\n";

    // start UDP discovery responder thread
    thread(udpDiscoveryResponder, 8888).detach();

    // thread to display active users periodically
    thread([](){
        while (true) {
            this_thread::sleep_for(chrono::seconds(2));
            lock_guard<mutex> lock(clientMutex);
            cout << "\n--- Active Users (" << clients.size() << ") --- ";
            for (auto &client : clients) cout << client.userName << " ";
            cout << " ---\n";
        }
    }).detach();

    // accept clients continuously
    while (true) {
        sockaddr_in clientAddr{};
        int clientAddrLen = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == INVALID_SOCKET) continue;

        // receive username from client
        char nameBuffer[512];
        int bytesReceived = recv(clientSocket, nameBuffer, sizeof(nameBuffer)-1, 0);
        if (bytesReceived <= 0) { closesocket(clientSocket); continue; }

        nameBuffer[bytesReceived] = '\0';
        string userName = trim(string(nameBuffer));
        if (userName.empty()) userName = "NoName";

        // ensure unique username
        lock_guard<mutex> lock(clientMutex);
        string uniqueUserName = userName;
        int suffix = 1;
        while (userSocketMap.find(uniqueUserName) != userSocketMap.end()) {
            uniqueUserName = userName + "_" + to_string(suffix++);
        }

        clients.push_back({clientSocket, uniqueUserName});
        userSocketMap[uniqueUserName] = clientSocket;

        cout << "[SERVER] User connected: " << uniqueUserName << " (" << inet_ntoa(clientAddr.sin_addr) << ")\n";

        // update user list
        sendUserListUnsafe();

        // start client handler thread
        thread(clientHandler, Client{clientSocket, uniqueUserName}).detach();
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}

