#define WIN32_LEAN_AND_MEAN
#undef UNICODE

#include <windows.h>

#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <tchar.h>
#include <TcpServer.hpp>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

TcpServer::TcpServer()
{
    // Initialize Winsock
    const auto startupResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (startupResult != 0) {
        printf("WSAStartup failed with error: %d\n", startupResult);
        return;
    }
    else
        printf("WSAStartup succeded\n");
}
TcpServer::~TcpServer()
{
    // cleanup
    closesocket(clientSocket);
    WSACleanup();
    printf("Closing TcpServer pipe\n");
}
void TcpServer::addService(const unsigned short addressFamily)
{
    this->addressFamily = addressFamily;
    // Create a SOCKET for the server to listen for client connections.
    listenSocket = socket(addressFamily, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return;
    }
    else
        printf("socket created successfully\n");
}
void TcpServer::awaitForConnection(const std::string& ipAddress, const unsigned short port)
{
    auto serviceAddress = createServiceAddress(ipAddress.c_str(), port);
    bindSocket(serviceAddress);
    listenToSocket();
    acceptIncomingClient();
    stopAwaitingForConnections();

}
std::string TcpServer::receiveMessage()
{
    const auto receiveResult = recv(clientSocket, recvbuf, recvbuflen, 0);
    if (receiveResult > 0) {
        printf("Bytes received: %d\n", receiveResult);
        std::string result;
        result.append(recvbuf, receiveResult);
        std::cout << "received message:"<< result << std::endl;
        return result;
    }
    else if (receiveResult == 0)
    {
        printf("Connection closing...\n");
        closesocket(clientSocket);
        return "CONNECTION_LOST";
    }
    else {
        return "TCP_RECV_EMPTY";
    }
}
bool TcpServer::sendMessage(const std::string& message)
{

    std::cout << "sending message: " << message << std::endl;
    const char* toSend = message.c_str();
    const auto sendResult = send(clientSocket, toSend, strlen(toSend), 0);
    if (sendResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(clientSocket);
        WSACleanup();
        return false;
    }
    printf("Bytes sent: %d\n", sendResult);
    return true;
}
void TcpServer::closeConnection()
{
    const auto shutdownResult = shutdown(clientSocket, SD_SEND);
    if (shutdownResult == SOCKET_ERROR)
        printf("shutdown failed with error: %d\n", WSAGetLastError());
    closesocket(clientSocket);
    WSACleanup();
}

sockaddr_in TcpServer::createServiceAddress(
    const char* ipAddress,
    const unsigned short port)
{
    sockaddr_in service{};
    service.sin_family = addressFamily;
    InetPton(AF_INET, _T(ipAddress), &service.sin_addr.s_addr);
    service.sin_port = htons(port);
    memset(&(service.sin_zero), '\0', 8);
    printf("AF_INET (IPv4)\n");
    printf("\tIPv4 address %s\n", inet_ntoa(service.sin_addr));
    return service;
}
void TcpServer::bindSocket(sockaddr_in& service)
{
    // Setup the TCP listening socket 
    printf("\tbindTcpSocket to: %s\n", inet_ntoa(service.sin_addr));
    printf("\tbindTcpSocket to: %d\n", service.sin_port);
    printf("\tbindTcpSocket to: %d\n", service.sin_family);
    const auto bindResult = bind(listenSocket, (struct sockaddr*)&service, sizeof(service));
    if (bindResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return;
    }
    printf("bind successfull\n");
}
void TcpServer::listenToSocket()
{
    const auto listenResult = listen(listenSocket, SOMAXCONN);
    if (listenResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return;
    }
    printf("listen successfully\n");
}
void TcpServer::acceptIncomingClient()
{
    // Accept a client socket
    clientSocket = accept(listenSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return;
    }
    printf("accepted successfully\n");
    u_long nonBlockingSocketMode = 1;
    auto clientSetNoBlockUntilMessageAppearsResult = ioctlsocket(clientSocket, FIONBIO, &nonBlockingSocketMode);
    if (clientSetNoBlockUntilMessageAppearsResult == SOCKET_ERROR) {
        std::cerr << "ioctlsocket failed with error: " << clientSetNoBlockUntilMessageAppearsResult << std::endl;
        closesocket(clientSocket);
        WSACleanup();
    }
}
void TcpServer::stopAwaitingForConnections()
{
    // No longer need server socket
    closesocket(listenSocket);
}
