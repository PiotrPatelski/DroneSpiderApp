#pragma once

#include <winsock2.h>
#include <string>

#define DEFAULT_BUFLEN 512

class UdpServer
{
public:
    UdpServer();
    ~UdpServer();
    void addService(
        const unsigned short addressFamily,
        const std::string& ipAddress, 
        const unsigned short port);
    std::string receiveMessage();
    void sendMessage(const std::string& message);
    void closeConnection();
private:
    sockaddr_in createServiceAddress(
        const unsigned short adressFamily,
        const char* ipAddress,
        const unsigned short port);
    void bindSocket(sockaddr_in& service);
private:
    WSADATA wsaData;
    unsigned short addressFamily{ AF_UNSPEC };
    SOCKET serverSocket = INVALID_SOCKET;
    sockaddr_in clientAddress;
    int clientAddressLength{ sizeof(clientAddress) };
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen{ DEFAULT_BUFLEN };
};
