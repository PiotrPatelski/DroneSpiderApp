#define WIN32_LEAN_AND_MEAN
#undef UNICODE
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <tchar.h>
#include <TcpClient.hpp>

#pragma comment (lib, "Ws2_32.lib")

TcpClient::TcpClient()
{
    const auto startupResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (startupResult != 0) {
        printf("WSAStartup failed with error: %d\n", startupResult);
        return;
    }
    else
        printf("WSAStartup succeded\n");
}

TcpClient::~TcpClient()
{
    closesocket(connectSocket);
    WSACleanup();
}

void TcpClient::addService(const unsigned short addressFamily)
{
    this->addressFamily = addressFamily;
    connectSocket = socket(addressFamily, SOCK_STREAM, IPPROTO_TCP);
    if (connectSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return;
    }
    else
        printf("socket created successfully\n");
}

bool TcpClient::connectToServer(const std::string& ipAddress, const unsigned short port)
{
    sockaddr_in serviceAddress = createServiceAddress(ipAddress.c_str(), port);
    std::cout << "Connecting..." << std::endl;
    const auto connectResult = connect(connectSocket, (SOCKADDR*)&serviceAddress, sizeof(serviceAddress));
    if (connectResult == SOCKET_ERROR) {
        closesocket(connectSocket);
        std::cout << "Unable to connect to server: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return false;
    }
    return true;
}

std::string TcpClient::receiveMessage()
{
    const auto receiveResult = recv(connectSocket, recvbuf, recvbuflen, 0);
    if (receiveResult > 0) {
        printf("Bytes received: %d\n", receiveResult);
        std::string result;
        result.append(recvbuf, receiveResult);
        std::cout << "received message: "<< result << std::endl;
        return result;
    }
    else if (receiveResult == 0)
    {
        printf("Connection closing...\n");
        return "TCP_CONNECTION_LOST";
    }
    else {
        printf("recv failed with error: %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return "TCP_RECV_ERROR";
    }
}

bool TcpClient::sendMessage(const std::string& message)
{

    std::cout << "sending message: " << message << std::endl;
    const char* toSend = message.c_str();
    const auto sendResult = send(connectSocket, toSend, strlen(toSend), 0);
    if (sendResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return false;
    }
    printf("Bytes sent: %d\n", sendResult);
    return true;
}

void TcpClient::closeConnection()
{
    const auto shutdownResult = shutdown(connectSocket, SD_SEND);
    if (shutdownResult == SOCKET_ERROR)
        printf("shutdown failed with error: %d\n", WSAGetLastError());
    closesocket(connectSocket);
    WSACleanup();
}

sockaddr_in TcpClient::createServiceAddress(
    const char* ipAddress,
    const unsigned short port)
{
    sockaddr_in service;
    service.sin_family = addressFamily;
    InetPton(addressFamily, _T(ipAddress), &service.sin_addr.s_addr);
    service.sin_port = htons(port);
    memset(&(service.sin_zero), '\0', 8);
    printf("AF_INET (IPv4)\n");
    printf("\tIPv4 address %s\n", inet_ntoa(service.sin_addr));
    return service;
}

