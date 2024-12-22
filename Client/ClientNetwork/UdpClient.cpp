#define WIN32_LEAN_AND_MEAN
#undef UNICODE
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <tchar.h>
#include <UdpClient.hpp>

#pragma comment (lib, "Ws2_32.lib")

UdpClient::UdpClient()
{
    const auto startupResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (startupResult != 0) {
        printf("WSAStartup failed with error: %d\n", startupResult);
        return;
    }
    else
        printf("WSAStartup succeded\n");
}
UdpClient::~UdpClient()
{
    closeConnection();
}
void UdpClient::addService(
    const unsigned short addressFamily, 
    const std::string& ipAddress, 
    const unsigned short port)
{
    this->addressFamily = addressFamily;
    clientSocket = socket(addressFamily, SOCK_DGRAM, IPPROTO_UDP);
    if (clientSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return;
    }
    else
        printf("socket created successfully\n");
    serviceAddress = createServiceAddress(ipAddress.c_str(), port);
}

std::string UdpClient::receiveMessage()
{
    const auto receiveResult = recvfrom(
        clientSocket,
        recvbuf,
        recvbuflen,
        0,
        (sockaddr*)&serviceAddress,
        &serviceAddressLength);
    char str[INET_ADDRSTRLEN];
    inet_ntop(addressFamily, &serviceAddress.sin_addr, str, INET_ADDRSTRLEN);
    std::cout << "Incomming message from: " << str << std::endl;
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
        return "UDP_CONNECTION_LOST";
    }
    else {
        printf("recv failed with error: %d\n", WSAGetLastError());
        closeConnection();
        return "UDP_RECV_ERROR";
    }
}
void UdpClient::sendMessage(const std::string& message)
{
    char str[INET_ADDRSTRLEN];
    inet_ntop(addressFamily, &serviceAddress.sin_addr, str, INET_ADDRSTRLEN);
    std::cout << "Sending name to: " << str << std::endl;
    std::cout << "sending message: " << message << std::endl;
    const char* toSend = message.c_str();
    sendto(clientSocket, toSend, strlen(toSend), 0, (struct sockaddr*)&serviceAddress, serviceAddressLength);
}
void UdpClient::closeConnection()
{
    closesocket(clientSocket);
    WSACleanup();
}

sockaddr_in UdpClient::createServiceAddress(
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

