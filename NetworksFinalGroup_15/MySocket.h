#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <stdexcept>
#include <cstring>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")

// Global enumerations and a constant for default buffer size.
enum SocketType { CLIENT, SERVER };
enum ConnectionType { TCP, UDP };
static const int DEFAULT_SIZE = 1024;

class MySocket {
private:
    char* Buffer;               // Dynamically allocated RAW buffer.
    SOCKET WelcomeSocket;       // Listening socket for a TCP server.
    SOCKET ConnectionSocket;    // Socket used for client/server communications.
    struct sockaddr_in SvrAddr; // Structure to store connection information.
    SocketType mySocket;        // Indicates if this is a CLIENT or SERVER socket.
    string IPAddr;               // IPv4 address.
    int Port;                   // Port number.
    ConnectionType connectionType; // TCP or UDP.
    bool bTCPConnect;           // Indicates if a TCP connection is established.
    int MaxSize;                // Maximum buffer size.

public:
    // Constructor: configures the socket, sets IP and port, and allocates the buffer.
    // If an invalid size is provided, DEFAULT_SIZE is used.
    MySocket(SocketType type, string ip, unsigned int port, ConnectionType connType, unsigned int maxSize);

    // Destructor: cleans up dynamically allocated memory and closes sockets.
    ~MySocket();

    // Establish a TCP connection (for TCP only).
    void ConnectTCP();
    // Disconnect a TCP connection.
    void DisconnectTCP();
    // Transmit a block of RAW data.
    void SendData(const char* data, int numBytes);
    // Receive data into an external buffer and return the number of bytes received.
    int GetData(char* destBuffer);

    // Getters and setters for IP and port.
    string GetIPAddr();
    void SetIPAddr(string newIP);
    void SetPort(int newPort);
    int GetPort();

    // Get and set the socket type.
    SocketType GetType();
    void SetType(SocketType type);
};

