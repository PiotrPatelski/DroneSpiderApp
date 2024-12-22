#include "UserConnectionProxy.hpp"
#include "Serializer.hpp"
#include "Deserializer.hpp"
#include <iostream>
#include <QMessageBox>

UserConnectionProxy::UserConnectionProxy(std::unique_ptr<TcpClient> tcpClient, std::unique_ptr<UdpClient> udpClient)
    : tcpClient(std::move(tcpClient)),
    udpClient(std::move(udpClient))
{
}

UserConnectionProxy::UserMessageVariant UserConnectionProxy::receiveMessage()
{
    std::lock_guard<std::mutex> lock(endpointMutex);
    const auto receivedTcpMessage = tcpClient->receiveMessage();
    if (receivedTcpMessage != "TCP_RECV_EMPTY") {
        return deserializeTcp(receivedTcpMessage);
    }
    const auto receivedUdpMessage = udpClient->receiveMessage();
    if (receivedUdpMessage != "UDP_RECV_EMPTY") {
        return deserializeUdp(receivedUdpMessage);
    }
    return std::nullopt;
}

UserConnectionProxy::UserMessageVariant UserConnectionProxy::receiveUdpMessage()
{
    std::lock_guard<std::mutex> lock(endpointMutex);
    const auto receivedUdpMessage = udpClient->receiveMessage();
    if (receivedUdpMessage != "UDP_RECV_EMPTY") {
        return deserializeUdp(receivedUdpMessage);
    }
    return std::nullopt;
}

void UserConnectionProxy::sendMessage(const MoveRequest& moveRequest)
{
    std::lock_guard<std::mutex> lock(endpointMutex);
    tcpClient->sendMessage(serialize(moveRequest));
}

void UserConnectionProxy::sendMessage(const RotateRequest& rotateRequest)
{
    std::lock_guard<std::mutex> lock(endpointMutex);
    tcpClient->sendMessage(serialize(rotateRequest));
}

void UserConnectionProxy::sendMessage(const StatusRequest& statusRequest)
{
    std::lock_guard<std::mutex> lock(endpointMutex);
    tcpClient->sendMessage(serialize(statusRequest));
}

void UserConnectionProxy::sendMessage(const EchoRequest& echoRequest)
{
    std::lock_guard<std::mutex> lock(endpointMutex);
    tcpClient->sendMessage(serialize(echoRequest));
}

void UserConnectionProxy::sendMessage(const ShutdownServerCommand& shutdownCommand)
{
    std::lock_guard<std::mutex> lock(endpointMutex);
    tcpClient->sendMessage(serialize(shutdownCommand));
}

void UserConnectionProxy::sendMessage(const MoveUp& moveUp)
{
    std::lock_guard<std::mutex> lock(endpointMutex);
    udpClient->sendMessage(serialize(moveUp));
}

void UserConnectionProxy::sendMessage(const MoveDown& moveDown)
{
    std::lock_guard<std::mutex> lock(endpointMutex);
    udpClient->sendMessage(serialize(moveDown));
}

void UserConnectionProxy::sendMessage(const MoveLeft& moveLeft)
{
    std::lock_guard<std::mutex> lock(endpointMutex);
    udpClient->sendMessage(serialize(moveLeft));
}

void UserConnectionProxy::sendMessage(const MoveRight& moveRight)
{
    std::lock_guard<std::mutex> lock(endpointMutex);
    udpClient->sendMessage(serialize(moveRight));
}

void UserConnectionProxy::close(const std::string& cause)
{
    std::lock_guard<std::mutex> lock(endpointMutex);
    tcpClient->sendMessage(cause);
    tcpClient->closeConnection();
    udpClient->closeConnection();
}

UserConnectionProxy::UserMessageVariant UserConnectionProxy::deserializeTcp(
    const std::string& receivedMessage) {

    const auto messageHeader = getMessageHeader(receivedMessage);

    if (messageHeader == "ECHO_CFM")
    {
        return deserializeEchoCfm(receivedMessage);
    }
    else if (messageHeader == "MOVE_DONE")
    {
        return deserializeMoveDone(receivedMessage);
    }
    else if (messageHeader == "ROTATE_DONE")
    {
        return deserializeRotateDone(receivedMessage);;
    }
    else if (messageHeader == "STATUS_IND")
    {
        return deserializeStatusInd(receivedMessage);
    }
    else if (messageHeader == "ADMIN_INITIATED_SHUTDOWN_CFM")
    {
        auto deserializedMsg = deserializeShutdownServerCfm(receivedMessage);
        std::cout << "UserConnectionProxy:: tcpClient graceful shutdown due to : "
            << messageHeader
            << " received from client" << std::endl;
        return deserializedMsg;
    }
    else if (messageHeader == "CONNECTION_LOST")
    {
        std::cout << "Closing connection due to : "
            << messageHeader
            << " received from server" << std::endl;
        return deserializeConnectionLostInd(receivedMessage);
    }
    else
    {
        std::cout << "Received unhandled message: " << messageHeader << std::endl;
        return std::nullopt;
    }
}

UserConnectionProxy::UserMessageVariant UserConnectionProxy::deserializeUdp(
    const std::string& receivedMessage) {
    if (receivedMessage == "CONNECTION_LOST")
    {
        return deserializeConnectionLostInd(receivedMessage);
    }
    else
    {
        return std::nullopt;
    }
}