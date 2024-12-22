#include "DroneConnectionStub.hpp"
#include "Serializer.hpp"
#include "Deserializer.hpp"
#include <iostream>

DroneConnectionStub::DroneConnectionStub(std::unique_ptr<TcpServer> tcpServer, std::unique_ptr<UdpServer> udpServer)
    : DroneConnectionProxy(std::move(tcpServer), std::move(udpServer))
{
    statusThread = std::thread(&DroneConnectionStub::startDroneStatusThread, this);
}

void DroneConnectionStub::startDroneStatusThread() {
    std::cout << "DroneConnectionStub:: startDroneStatusThread\n";
    while (!shutdown) {
        std::lock_guard<std::mutex> lock(endpointMutex);
        messageQueue.push_back(DroneStatus{ "DRONE_STATUS", context.id, context });
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
    std::cout << "DroneConnectionStub:: stopDroneStatusThread\n";
}

DroneConnectionStub::DroneMessageVariant DroneConnectionStub::receiveMessage(const unsigned short instanceId)
{
    std::lock_guard<std::mutex> lock(endpointMutex);
    if (messageQueue.empty())
    {
        return std::nullopt;
    }
    const auto receivedMessage = messageQueue.front();
    messageQueue.pop_front();
    return receivedMessage;
}

std::optional<MoveDone> DroneConnectionStub::receiveMoveDone(const unsigned short instanceId, const unsigned short commandingAdminId)
{
    return MoveDone{ "MOVE_DONE", instanceId, commandingAdminId, TransformationResult::FINISHED };
}

std::optional<RotateDone> DroneConnectionStub::receiveRotateDone(const unsigned short instanceId, const unsigned short commandingAdminId)
{
    return RotateDone{ "ROTATE_DONE", instanceId, commandingAdminId, TransformationResult::FINISHED };
}

void DroneConnectionStub::sendMessage(const MoveCommand& moveCommand)
{
    std::cout << "DroneConnectionStub:: received MoveCommand{" << moveCommand.xPosition << ", " << moveCommand.yPosition << "}"<<std::endl;
    std::lock_guard<std::mutex> lock(endpointMutex);
    messageQueue.push_back(MoveCommandAck{ "MOVE_COMMAND_ACK", moveCommand.receiverId});
    std::this_thread::sleep_for(std::chrono::seconds(2));
    context.position.x += moveCommand.xPosition;
    context.position.y += moveCommand.yPosition;
}

void DroneConnectionStub::sendMessage(const RotateCommand& rotateCommand)
{
    std::lock_guard<std::mutex> lock(endpointMutex);
    messageQueue.push_back(RotateCommandAck{ "ROTATE_COMMAND_ACK", rotateCommand.receiverId });
    context.orientation += rotateCommand.angle;
}

void DroneConnectionStub::sendMessage(const MoveUp& moveUp)
{
    std::lock_guard<std::mutex> lock(endpointMutex);
    context.position.y += 1;
    messageQueue.push_back(MovedUp{ "MOVED_UP", moveUp.receiverId, static_cast<unsigned short>(moveUp.senderId) });
}

void DroneConnectionStub::sendMessage(const MoveDown& moveDown)
{
    std::lock_guard<std::mutex> lock(endpointMutex);
    context.position.y -= 1;
    messageQueue.push_back(MovedDown{ "MOVED_DOWN", moveDown.receiverId, static_cast<unsigned short>(moveDown.senderId) });
}

void DroneConnectionStub::sendMessage(const MoveLeft& moveLeft)
{
    std::lock_guard<std::mutex> lock(endpointMutex);
    context.position.x -= 1;
    messageQueue.push_back(MovedLeft{ "MOVE_LEFT", moveLeft.receiverId, static_cast<unsigned short>(moveLeft.senderId) });
}

void DroneConnectionStub::sendMessage(const MoveRight& moveRight)
{
    std::lock_guard<std::mutex> lock(endpointMutex);
    context.position.x += 1;
    messageQueue.push_back(MovedRight{ "MOVE_RIGHT", moveRight.receiverId, static_cast<unsigned short>(moveRight.senderId) });
}

void DroneConnectionStub::close(const std::string& cause)
{
    shutdown = true;
    if (statusThread.joinable())
    {
        statusThread.join();
    }
}