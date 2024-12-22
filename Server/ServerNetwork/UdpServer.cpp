#define WIN32_LEAN_AND_MEAN
#undef UNICODE

#include <windows.h>

#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <tchar.h>
#include <UdpServer.hpp>

#pragma comment (lib, "Ws2_32.lib")

UdpServer::UdpServer()
{
    const auto startupResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (startupResult != 0) {
        printf("WSAStartup failed with error: %d\n", startupResult);
        return;
    }
    else
        printf("WSAStartup succeded\n");
}
UdpServer::~UdpServer()
{
    closeConnection();
    std::cout << "closing Udp socket: " << std::endl;
}
void UdpServer::addService(
    const unsigned short addressFamily,
    const std::string& ipAddress,
    const unsigned short port)
{
    this->addressFamily = addressFamily;
    serverSocket = socket(addressFamily, SOCK_DGRAM, IPPROTO_UDP);
    if (serverSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return;
    }
    printf("socket created successfully\n");
    auto serviceAddress = createServiceAddress(addressFamily, ipAddress.c_str(), port);
    bindSocket(serviceAddress);
    u_long nonBlockingSocketMode = 1;
    auto clientSetNoBlockUntilMessageAppearsResult = ioctlsocket(serverSocket, FIONBIO, &nonBlockingSocketMode);
    if (clientSetNoBlockUntilMessageAppearsResult == SOCKET_ERROR) {
        std::cerr << "ioctlsocket failed with error: " << clientSetNoBlockUntilMessageAppearsResult << std::endl;
        closesocket(serverSocket);
        WSACleanup();
    }
}

std::string UdpServer::receiveMessage()
{  
    const auto receiveResult = recvfrom(
        serverSocket, 
        recvbuf, 
        recvbuflen, 
        0,
        (struct sockaddr*) & clientAddress,
        &clientAddressLength);
    char str[INET_ADDRSTRLEN];
    inet_ntop(addressFamily, &clientAddress.sin_addr, str, INET_ADDRSTRLEN);
    if (receiveResult > 0) {
        std::cout << "Incomming message from: " << str << std::endl;
        printf("Bytes received: %d\n", receiveResult);
        std::string result;
        result.append(recvbuf, receiveResult);
        std::cout << "received message:"<< result << std::endl;
        return result;
    }
    else if (receiveResult == 0)
    {
        printf("Connection closing...\n");
        closeConnection();
        return "CONNECTION_LOST";
    }
    else {
        return "UDP_RECV_EMPTY";
    }
}
void UdpServer::sendMessage(const std::string& message)
{
    std::cout << "sending message: " << message << std::endl;
    const char* toSend = message.c_str();
    sendto(serverSocket, toSend, strlen(toSend), 0, (sockaddr*)&clientAddress, clientAddressLength);
}
void UdpServer::closeConnection()
{
    closesocket(serverSocket);
    WSACleanup();
}

sockaddr_in UdpServer::createServiceAddress(
    const unsigned short adressFamily,
    const char* ipAddress,
    const unsigned short port)
{
    sockaddr_in service{};
    service.sin_family = addressFamily;
    InetPton(adressFamily, _T(ipAddress), &service.sin_addr.s_addr);
    service.sin_port = htons(port);
    memset(&(service.sin_zero), '\0', 8);
    printf("AF_INET (IPv4)\n");
    printf("\tIPv4 address %s\n", inet_ntoa(service.sin_addr));
    return service;
}
void UdpServer::bindSocket(sockaddr_in& service)
{
    printf("\tbindSocket to: %s\n", inet_ntoa(service.sin_addr));
    printf("\tbindSocket to: %d\n", service.sin_port);
    printf("\tbindSocket to: %d\n", service.sin_family);
    const auto bindResult = bind(serverSocket, (sockaddr*)&service, sizeof(service));
    if (bindResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        closeConnection();
        return;
    }
    else
        printf("bind successfull\n");

}
