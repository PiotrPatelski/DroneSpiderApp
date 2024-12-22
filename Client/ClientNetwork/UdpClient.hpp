#pragma once
#include <winsock2.h>

#define DEFAULT_BUFLEN 512

class UdpClient
{
public:
    UdpClient();
    ~UdpClient();
    void addService(
        const unsigned short addressFamily,
        const std::string& ipAddress,
        const unsigned short port);
    std::string receiveMessage();
    void sendMessage(const std::string& message);
    void closeConnection();
private:

    sockaddr_in createServiceAddress(
        const char* ipAddress,
        const unsigned short port);
private:
    WSADATA wsaData;
    unsigned short addressFamily = AF_UNSPEC;
    SOCKET clientSocket = INVALID_SOCKET;
    sockaddr_in serviceAddress;
    int serviceAddressLength{ sizeof(serviceAddress) };
    char recvbuf[DEFAULT_BUFLEN];
    const int recvbuflen = DEFAULT_BUFLEN;
};
