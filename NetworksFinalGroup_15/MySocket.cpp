#include "MySocket.h"
#include <stdexcept>
#include <string>
#include <cstring>
using namespace std; 

MySocket::MySocket(SocketType type, string ip, unsigned int port, ConnectionType connType, unsigned int maxSize)
    : Buffer(nullptr), WelcomeSocket(INVALID_SOCKET), ConnectionSocket(INVALID_SOCKET),
    mySocket(type), IPAddr(ip), Port(static_cast<int>(port)), connectionType(connType),
    bTCPConnect(false), MaxSize((maxSize > 0) ? static_cast<int>(maxSize) : DEFAULT_SIZE)
{
    // Initialize Winsock.
    WSADATA wsaData;
    int wsResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsResult != 0)
        throw std::runtime_error("WSAStartup failed");

    // Allocate the communication buffer.
    Buffer = new char[MaxSize];

    // Prepare the server address structure.
    ZeroMemory(&SvrAddr, sizeof(SvrAddr));
    SvrAddr.sin_family = AF_INET;
    SvrAddr.sin_port = htons(Port);
    inet_pton(AF_INET, IPAddr.c_str(), &SvrAddr.sin_addr);

    // Create a socket based on ConnectionType.
    if (connectionType == TCP) {
        SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock == INVALID_SOCKET)
            throw runtime_error("Failed to create TCP socket");

        if (mySocket == SERVER) {
            WelcomeSocket = sock;
            if (bind(WelcomeSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR)
                throw runtime_error("TCP Bind failed");
            if (listen(WelcomeSocket, SOMAXCONN) == SOCKET_ERROR)
                throw runtime_error("TCP Listen failed");
        }
        else { // CLIENT
            ConnectionSocket = sock;
        }
    }
    else { // UDP
        SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (sock == INVALID_SOCKET)
            throw runtime_error("Failed to create UDP socket");
        ConnectionSocket = sock;
        if (mySocket == SERVER) {
            if (bind(ConnectionSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR)
                throw runtime_error("UDP Bind failed");
        }
    }
}

MySocket::~MySocket() {
    if (Buffer) {
        delete[] Buffer;
        Buffer = nullptr;
    }
    if (ConnectionSocket != INVALID_SOCKET) {
        closesocket(ConnectionSocket);
        ConnectionSocket = INVALID_SOCKET;
    }
    if (WelcomeSocket != INVALID_SOCKET) {
        closesocket(WelcomeSocket);
        WelcomeSocket = INVALID_SOCKET;
    }
    WSACleanup();
}

void MySocket::ConnectTCP() {
    if (connectionType != TCP)
        throw runtime_error("ConnectTCP called on a UDP socket");
    if (mySocket == CLIENT) {
        if (connect(ConnectionSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR)
            throw runtime_error("TCP Connect failed");
        bTCPConnect = true;
    }
    else { // SERVER
        SOCKET clientSocket = accept(WelcomeSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET)
            throw runtime_error("TCP Accept failed");
        ConnectionSocket = clientSocket;
        bTCPConnect = true;
    }
}

void MySocket::DisconnectTCP() {
    if (connectionType != TCP)
        throw runtime_error("DisconnectTCP called on a UDP socket");
    if (!bTCPConnect)
        return;
    shutdown(ConnectionSocket, SD_SEND);
    closesocket(ConnectionSocket);
    ConnectionSocket = INVALID_SOCKET;
    bTCPConnect = false;
}

void MySocket::SendData(const char* data, int numBytes) {
    if (connectionType == TCP) {
        if (send(ConnectionSocket, data, numBytes, 0) == SOCKET_ERROR)
            throw runtime_error("TCP Send failed");
    }
    else { // UDP
        int addrLen = sizeof(SvrAddr);
        if (sendto(ConnectionSocket, data, numBytes, 0, (struct sockaddr*)&SvrAddr, addrLen) == SOCKET_ERROR)
            throw runtime_error("UDP sendto failed");
    }
}

int MySocket::GetData(char* destBuffer) {
    int bytesReceived = 0;
    if (connectionType == TCP) {
        bytesReceived = recv(ConnectionSocket, Buffer, MaxSize, 0);
    }
    else { // UDP
        int addrLen = sizeof(SvrAddr);
        bytesReceived = recvfrom(ConnectionSocket, Buffer, MaxSize, 0, (struct sockaddr*)&SvrAddr, &addrLen);
    }
    if (bytesReceived == SOCKET_ERROR)
        throw runtime_error("Receive failed");
    memcpy(destBuffer, Buffer, bytesReceived);
    return bytesReceived;
}

string MySocket::GetIPAddr() {
    return IPAddr;
}

void MySocket::SetIPAddr(string newIP) {
    if (bTCPConnect)
        throw runtime_error("Cannot change IP address after connection established");
    IPAddr = newIP;
    inet_pton(AF_INET, IPAddr.c_str(), &SvrAddr.sin_addr);
}

void MySocket::SetPort(int newPort) {
    if (bTCPConnect)
        throw runtime_error("Cannot change port after connection established");
    Port = newPort;
    SvrAddr.sin_port = htons(Port);
}

int MySocket::GetPort() {
    return Port;
}

SocketType MySocket::GetType() {
    return mySocket;
}

void MySocket::SetType(SocketType type) {
    if (bTCPConnect || (mySocket == SERVER && WelcomeSocket != INVALID_SOCKET))
        throw runtime_error("Cannot change socket type after connection is established");
    mySocket = type;
}

