#include "AdminConnectionProxy.hpp"
#include "Serializer.hpp"
#include "Deserializer.hpp"
#include <iostream>

AdminConnectionProxy::AdminConnectionProxy(std::unique_ptr<TcpServer> tcpServer, std::unique_ptr<UdpServer> udpServer)
    : tcpServer(std::move(tcpServer)),
    udpServer(std::move(udpServer))
{
    std::cout << "AdminConnectionProxy:: init\n";
}

AdminConnectionProxy::AdminMessageVariant AdminConnectionProxy::receiveMessage()
{
    std::lock_guard<std::mutex> lock(endpointMutex);
    const auto receivedTcpMessage = tcpServer->receiveMessage();
    if (receivedTcpMessage != "TCP_RECV_EMPTY") {
        std::cout << "AdminConnectionProxy:: new TCP message received\n";
        return deserializeTcp(receivedTcpMessage);
    }

    const auto receivedUdpMessage = udpServer->receiveMessage();
    if (receivedUdpMessage != "UDP_RECV_EMPTY") {
        std::cout << "AdminConnectionProxy:: new UDP message received\n";
        return deserializeUdp(receivedUdpMessage);
    }

    return std::nullopt;
}

void AdminConnectionProxy::sendMessage(const MoveDone& moveDone)
{
    std::cout << "AdminConnectionProxy:: sending MoveDone to Admin client\n";
    std::lock_guard<std::mutex> lock(endpointMutex);
    tcpServer->sendMessage(serialize(moveDone));
    std::cout << "AdminConnectionProxy:: sent MoveDone by TCP\n";
}

void AdminConnectionProxy::sendMessage(const RotateDone& rotateDone)
{
    std::cout << "AdminConnectionProxy:: sending RotateDone to Admin client\n";
    std::lock_guard<std::mutex> lock(endpointMutex);
    tcpServer->sendMessage(serialize(rotateDone));
    std::cout << "AdminConnectionProxy:: sent RotateDone by TCP\n";
}

void AdminConnectionProxy::sendMessage(const StatusInd& statusInd)
{
    std::cout << "AdminConnectionProxy:: sending StatusInd to Admin client\n";
    std::lock_guard<std::mutex> lock(endpointMutex);
    tcpServer->sendMessage(serialize(statusInd));
    std::cout << "AdminConnectionProxy:: sent StatusInd by TCP\n";
}

void AdminConnectionProxy::sendMessage(const EchoCfm& echoCfm)
{
    std::cout << "AdminConnectionProxy:: sending EchoCfm to Admin client\n";
    std::lock_guard<std::mutex> lock(endpointMutex);
    tcpServer->sendMessage(serialize(echoCfm));
    std::cout << "AdminConnectionProxy:: sent EchoCfm by TCP\n";
}

void AdminConnectionProxy::sendMessage(const MovedUp& movedUp)
{
    std::cout << "AdminConnectionProxy:: sending MovedUp to Admin client\n";
    std::lock_guard<std::mutex> lock(endpointMutex);
    udpServer->sendMessage(movedUp.messageHeader);
    std::cout << "AdminConnectionProxy:: sent MovedUp by UDP\n";
}

void AdminConnectionProxy::sendMessage(const MovedDown& movedDown)
{
    std::cout << "AdminConnectionProxy:: sending MovedDown to Admin client\n";
    std::lock_guard<std::mutex> lock(endpointMutex);
    udpServer->sendMessage(movedDown.messageHeader);
    std::cout << "AdminConnectionProxy:: sent MovedDown by UDP\n";
}

void AdminConnectionProxy::sendMessage(const MovedLeft& movedLeft)
{
    std::cout << "AdminConnectionProxy:: sending MovedLeft to Admin client\n";
    std::lock_guard<std::mutex> lock(endpointMutex);
    udpServer->sendMessage(movedLeft.messageHeader);
    std::cout << "AdminConnectionProxy:: sent MovedLeft by UDP\n";
}

void AdminConnectionProxy::sendMessage(const MovedRight& movedRight)
{
    std::cout << "AdminConnectionProxy:: sending MovedRight to Admin client\n";
    std::lock_guard<std::mutex> lock(endpointMutex);
    udpServer->sendMessage(movedRight.messageHeader);
    std::cout << "AdminConnectionProxy:: sent MovedRight by UDP\n";
}

void AdminConnectionProxy::close(const std::string& cause)
{
    std::cout << "AdminConnectionProxy:: sending cause to Admin client\n";
    std::lock_guard<std::mutex> lock(endpointMutex);
    tcpServer->sendMessage(cause);
    std::cout << "AdminConnectionProxy:: sent cause by TCP\n";
    tcpServer->closeConnection();
    udpServer->closeConnection();
}

AdminConnectionProxy::AdminMessageVariant AdminConnectionProxy::deserializeTcp(
    const std::string& receivedMessage) {

    const auto messageHeader = getMessageHeader(receivedMessage);
    std::cout << "AdminConnectionProxy:: deserializeTcp - header: "<< messageHeader<<std::endl;
    if (messageHeader == "ECHO_REQUEST")
    {
        return deserializeEchoRequest(receivedMessage);
    }
    else if (messageHeader == "MOVE_REQUEST")
    {
        return deserializeMoveRequest(receivedMessage);
    }
    else if (messageHeader == "ROTATE_REQUEST")
    {
        return deserializeRotateRequest(receivedMessage);
    }
    else if (messageHeader == "STATUS_REQUEST")
    {
        return deserializeStatusRequest(receivedMessage);
    }
    else if (messageHeader == "ADMIN_INITIATED_SHUTDOWN_COMMAND")
    { 
        std::cout << "TcpServer graceful shutdown due to : "
            << messageHeader
            << " received from client" << std::endl;
        return deserializeShutdownServerCommand(receivedMessage);
    }
    else if (messageHeader == "CONNECTION_LOST")
    {
        std::cout << "Closing connection due to : "
            << messageHeader
            << " received from client" << std::endl;
        return deserializeConnectionLostInd(receivedMessage);
    }
    else
    {
        std::cout << "Received unhandled message: " << messageHeader << std::endl;
        return std::nullopt;
    }
}

AdminConnectionProxy::AdminMessageVariant AdminConnectionProxy::deserializeUdp(
    const std::string& receivedMessage) {
    const auto messageHeader = getMessageHeader(receivedMessage);
    std::cout << "AdminConnectionProxy:: deserializeUdp - messageHeader: " << messageHeader << std::endl;
    if (messageHeader == "MOVE_UP")
    {
        return deserializeMoveUp(receivedMessage);
    }
    else if (messageHeader == "MOVE_DOWN")
    {
        return deserializeMoveDown(receivedMessage);
    }
    else if (messageHeader == "MOVE_LEFT")
    {
        return deserializeMoveLeft(receivedMessage);
    }
    else if (messageHeader == "MOVE_RIGHT")
    {
        return deserializeMoveRight(receivedMessage);
    }
    else if (messageHeader == "CONNECTION_LOST")
    {
        return deserializeConnectionLostInd(receivedMessage);
    }
    else
    {
        std::cout << "Received unhandled message: " << receivedMessage << std::endl;
        return std::nullopt;
    }
}