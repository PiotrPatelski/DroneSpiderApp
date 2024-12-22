#include "DroneConnectionProxy.hpp"
#include "Serializer.hpp"
#include "Deserializer.hpp"
#include <iostream>

static constexpr std::chrono::seconds defaultTimeoutValue{ 5 };

DroneConnectionProxy::DroneConnectionProxy(std::unique_ptr<TcpServer> tcpServer, std::unique_ptr<UdpServer> udpServer)
    : tcpServer(std::move(tcpServer)),
    udpServer(std::move(udpServer))
{
    std::cout << "DroneConnectionProxy:: init\n";
}

DroneConnectionProxy::DroneMessageVariant DroneConnectionProxy::receiveMessage(const unsigned short instanceId)
{
    std::lock_guard<std::mutex> lock(endpointMutex);
    const auto receivedTcpMessage = tcpServer->receiveMessage();
    if (receivedTcpMessage != "TCP_RECV_EMPTY") {
        std::cout << "DroneConnectionProxy:: new TCP message received\n";
        return deserializeTcp(receivedTcpMessage, instanceId);
    }

    const auto receivedUdpMessage = udpServer->receiveMessage();
    if (receivedUdpMessage != "UDP_RECV_EMPTY") {
        std::cout << "DroneConnectionProxy:: new UDP message received\n";
        return deserializeUdp(receivedUdpMessage, instanceId);
    }

    return std::nullopt;
}

bool DroneConnectionProxy::receivedMatches(const std::string& matcher)
{
    std::cout << "DroneConnectionProxy:: receivedMatches\n";
    auto startTime = std::chrono::steady_clock::now();
    std::lock_guard<std::mutex> lock(endpointMutex);
    while (true)
    {
        const auto receivedMessage = tcpServer->receiveMessage();
        const auto messageHeader = getMessageHeader(receivedMessage);
        if (messageHeader == matcher)
        {
            std::cout << "DroneConnectionProxy:: message matched\n";
            return true;
        }
        else if (std::chrono::steady_clock::now() - startTime >= defaultTimeoutValue)
        {
            std::cout << "DroneConnectionProxy: Timeout reached. No valid message received\n";

            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return false;
}

std::optional<MoveDone> DroneConnectionProxy::receiveMoveDone(const unsigned short instanceId, const unsigned short commandingAdminId)
{
    if (receivedMatches("MOVE_DONE"))
    {
        std::cout << "DroneConnectionProxy: received MoveDone from Drone Unit\n";
        return MoveDone{ "MOVE_DONE", instanceId, commandingAdminId, TransformationResult::FINISHED };
    }
    return std::nullopt;
}

std::optional<RotateDone> DroneConnectionProxy::receiveRotateDone(const unsigned short instanceId, const unsigned short commandingAdminId)
{
    if(receivedMatches("ROTATE_DONE"))
    {
        std::cout << "DroneConnectionProxy: received RotateDone from Drone Unit\n";
        return RotateDone{ "ROTATE_DONE", instanceId, commandingAdminId, TransformationResult::FINISHED };
    }
    return std::nullopt;
}

void DroneConnectionProxy::sendMessage(const MoveCommand& moveCommand)
{
    std::cout << "DroneConnectionProxy:: sending MoveCommand to Drone Unit\n";
    std::lock_guard<std::mutex> lock(endpointMutex);
    tcpServer->sendMessage(serialize(moveCommand));
    std::cout << "DroneConnectionProxy:: sent MoveCommand by TCP\n";
}

void DroneConnectionProxy::sendMessage(const RotateCommand& rotateCommand)
{
    std::cout << "DroneConnectionProxy:: sending RotateCommand to Drone Unit\n";
    std::lock_guard<std::mutex> lock(endpointMutex);
    tcpServer->sendMessage(serialize(rotateCommand));
    std::cout << "DroneConnectionProxy:: sent RotateCommand by TCP\n";
}

void DroneConnectionProxy::sendMessage(const MoveUp& moveUp)
{
    std::cout << "DroneConnectionProxy:: sending MoveUp to Drone Unit\n";
    std::lock_guard<std::mutex> lock(endpointMutex);
    udpServer->sendMessage("MOVE_UP");
    std::cout << "DroneConnectionProxy:: sent MoveUp by UDP\n";
}

void DroneConnectionProxy::sendMessage(const MoveDown& moveDown)
{
    std::cout << "DroneConnectionProxy:: sending MoveDown to Drone Unit\n";
    std::lock_guard<std::mutex> lock(endpointMutex);
    udpServer->sendMessage("MOVE_DOWN");
    std::cout << "DroneConnectionProxy:: sent MoveDown by UDP\n";
}

void DroneConnectionProxy::sendMessage(const MoveLeft& moveLeft)
{
    std::cout << "DroneConnectionProxy:: sending MoveLeft to Drone Unit\n";
    std::lock_guard<std::mutex> lock(endpointMutex);
    udpServer->sendMessage("MOVE_LEFT");
    std::cout << "DroneConnectionProxy:: sent MoveLeft by UDP\n";
}

void DroneConnectionProxy::sendMessage(const MoveRight& moveRight)
{
    std::cout << "DroneConnectionProxy:: sending MoveRight to Drone Unit\n";
    std::lock_guard<std::mutex> lock(endpointMutex);
    udpServer->sendMessage("MOVE_RIGHT");
    std::cout << "DroneConnectionProxy:: sent MoveRight by UDP\n";
}

void DroneConnectionProxy::close(const std::string& cause)
{
    std::cout << "DroneConnectionProxy:: sending cause to Drone client\n";
    std::lock_guard<std::mutex> lock(endpointMutex);
    tcpServer->sendMessage(cause);
    std::cout << "DroneConnectionProxy:: sent cause by TCP\n";
    tcpServer->closeConnection();
    udpServer->closeConnection();
}

DroneConnectionProxy::DroneMessageVariant DroneConnectionProxy::deserializeTcp(
    const std::string& receivedMessage, const unsigned short instanceId) {

    const auto messageHeader = getMessageHeader(receivedMessage);
    std::cout << "DroneConnectionProxy:: deserializeTcp - header: " << messageHeader << std::endl;
    if (messageHeader == "MOVE_COMMAND_ACK")
    {
        return MoveCommandAck{ messageHeader, instanceId };
    }
    else if (messageHeader == "ROTATE_COMMAND_ACK")
    {
        return RotateCommandAck{ messageHeader, instanceId };
    }
    else if (messageHeader == "DRONE_STATUS")
    {
        return DroneStatus{"DRONE_STATUS", instanceId, deserializeDroneContext(receivedMessage) };
    }
    else if (messageHeader == "CONNECTION_LOST")
    {
        std::cout << "Closing connection due to : "
            << messageHeader
            << " received from client" << std::endl;
        return ConnectionLostInd{ receivedMessage, instanceId };
    }
    else
    {
        std::cout << "Received unhandled message: " << messageHeader << std::endl;
        return std::nullopt;
    }
}

DroneConnectionProxy::DroneMessageVariant DroneConnectionProxy::deserializeUdp(
    const std::string& receivedMessage, const unsigned short instanceId) {
    ////////////////////////////////////NOTE//////////////////////////////////////
    //FOR NOW IT IS ASSUMED THAT MESSAGES COMMING FROM DRONE UNIT ARE HEADER ONLY!!!
    ////////////////////////////////////NOTE//////////////////////////////////////
    std::cout << "DroneConnectionProxy:: deserializeUdp - message: " << receivedMessage << std::endl;
    if (receivedMessage == "MOVED_UP")
    {
        return MovedUp{ receivedMessage, instanceId };
    }
    else if (receivedMessage == "MOVED_DOWN")
    {
        return MovedDown{ receivedMessage, instanceId };
    }
    else if (receivedMessage == "MOVED_LEFT")
    {
        return MovedLeft{ receivedMessage, instanceId };
    }
    else if (receivedMessage == "MOVED_RIGHT")
    {
        return MovedRight{ receivedMessage, instanceId };
    }
    else if (receivedMessage == "CONNECTION_LOST")
    {
        return ConnectionLostInd{ receivedMessage, instanceId };
    }
    else
    {
        std::cout << "Received unhandled message: " << receivedMessage << std::endl;
        return std::nullopt;
    }
}