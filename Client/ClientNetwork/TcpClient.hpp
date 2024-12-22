#pragma once
#include <winsock2.h>

#define DEFAULT_BUFLEN 512

class TcpClient
{
public:
    TcpClient();
    ~TcpClient();
    void addService(const unsigned short addressFamily);
    bool connectToServer(const std::string& ipAddress, const unsigned short port);
    std::string receiveMessage();
    bool sendMessage(const std::string& message);
    void closeConnection();
private:

    sockaddr_in createServiceAddress(
        const char* ipAddress,
        const unsigned short port);
private:
    WSADATA wsaData;
    SOCKET connectSocket = INVALID_SOCKET;

    unsigned short addressFamily = AF_UNSPEC;
    char recvbuf[DEFAULT_BUFLEN];
    const int recvbuflen = DEFAULT_BUFLEN;
};
