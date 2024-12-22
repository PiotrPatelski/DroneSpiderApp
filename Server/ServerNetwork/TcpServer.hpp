#pragma once

#include <winsock2.h>
#include <string>

#define DEFAULT_BUFLEN 512

class TcpServer
{
public:
    TcpServer();
    ~TcpServer();
    void addService(const unsigned short addressFamily);
    void awaitForConnection(const std::string& ipAddress, const unsigned short port);
    std::string receiveMessage();
    bool sendMessage(const std::string& message);
    void closeConnection();
private:

    sockaddr_in createServiceAddress(
        const char* ipAddress,
        const unsigned short port);
    void bindSocket(sockaddr_in& service);
    void listenToSocket();
    void acceptIncomingClient();
    void stopAwaitingForConnections();
private:
    WSADATA wsaData;
    unsigned short addressFamily = AF_UNSPEC;
    SOCKET listenSocket = INVALID_SOCKET;
    SOCKET clientSocket = INVALID_SOCKET;

    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
};
