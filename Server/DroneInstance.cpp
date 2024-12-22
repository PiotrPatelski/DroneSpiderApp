#include "DroneInstance.hpp"
#include "UnitContext.hpp"
#include "Message.hpp"
#include "Serializer.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>

DroneInstance::DroneInstance(std::unique_ptr<DroneConnectionProxy> connection, const unsigned short instanceId)
    : connection(std::move(connection)),
    context{ instanceId, UnitStatus::IDLE, {0.f, 0.f}, {0.f} } {
    std::cout << "DroneInstance:: init\n";
}

DroneInstance::~DroneInstance() {
    if (workerThread.joinable())
    {
        workerThread.join();
    }
    std::cout << "DroneInstance:: closing!\n";
}

void DroneInstance::run() {
    std::cout << "DroneInstance:: run!\n";
    while (!shutdown) {
        const auto receivedMessage = connection->receiveMessage(context.id);
        if (receivedMessage)
        {
            std::cout << "DroneInstance:: received new Message\n";
            std::visit([this](auto&& message) { handleMessage(message); }, *receivedMessage);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "DroneInstance stop" << std::endl;
}

void DroneInstance::start() {
    std::cout << "DroneInstance:: start instance thread\n";
    workerThread = std::thread(&DroneInstance::run, this);
}

void DroneInstance::handleMessage(const MoveCommand& moveCommand) {
    std::cout << "DroneInstance:: received "
        << moveCommand.messageHeader
        <<" from adminId: "
        << moveCommand.senderId
        <<", sending MOVE_COMMAND to drone client\n";
    commandingAdminId = moveCommand.senderId;
    connection->sendMessage(moveCommand);
}

void DroneInstance::handleMessage(const RotateCommand& rotateCommand) {
    std::cout << "DroneInstance:: received "
        << rotateCommand.messageHeader
        << " from adminId: "
        << rotateCommand.senderId
        << ", sending ROTATE_COMMAND to drone client\n";
    commandingAdminId = rotateCommand.senderId;
    connection->sendMessage(rotateCommand);
}

void DroneInstance::handleMessage(const MoveCommandAck& moveCommandAck) {
    std::cout << "DroneInstance:: target drone acknowledged received: "
        << moveCommandAck.messageHeader
        << " and proceeds to execute order\n";
    awaitMoveDone();
}

void DroneInstance::handleMessage(const RotateCommandAck& rotateCommandAck) {
    std::cout << "DroneInstance:: target drone acknowledged received: "
        << rotateCommandAck.messageHeader
        << " and proceeds to execute order\n";
    awaitRotateDone();
}

void DroneInstance::handleMessage(const DroneStatus& droneStatus) {
    std::cout << "DroneInstance:: target drone status received: "
        << droneStatus.messageHeader <<"{" 
        << droneStatus.data.id<< ", {"
        << droneStatus.data.position.x << ", "<< droneStatus.data.position.x << "}, "
        << droneStatus.data.orientation << ", "
        << droneStatus.data.status <<"}"
        << " and proceeds to execute order\n";
    context = droneStatus.data;
    std::cout << "DroneInstance:: current drone status: "
        << "{"
        << context.id << ", {"
        << context.position.x << ", " << context.position.x << "}, "
        << context.orientation << ", "
        << context.status << "}\n";
}

void DroneInstance::handleMessage(const ConnectionLostInd& connectionLostInd) {
    std::cout << "DroneInstance:: Closing connection due to : "
        << connectionLostInd.messageHeader
        << " received from client\n";
}

void DroneInstance::handleMessage(const MoveUp& moveUp) {
    std::cout << "DroneInstance:: sending moveUp to Drone Unit\n";
    connection->sendMessage(moveUp);
}

void DroneInstance::handleMessage(const MoveDown& moveDown) {
    std::cout << "DroneInstance:: sending moveDown to Drone Unit\n";
    connection->sendMessage(moveDown);
}

void DroneInstance::handleMessage(const MoveLeft& moveLeft) {
    std::cout << "DroneInstance:: sending moveLeft to Drone Unit\n";
    connection->sendMessage(moveLeft);
}

void DroneInstance::handleMessage(const MoveRight& moveRight) {
    std::cout << "DroneInstance:: sending moveRight to Drone Unit\n";
    connection->sendMessage(moveRight);
}

void DroneInstance::handleMessage(const MovedUp& movedUp) {
    std::cout << "DroneInstance:: received "<< movedUp.messageHeader<<" from Unit\n";
    pendingMessages.push(DroneMessage{ movedUp });
}

void DroneInstance::handleMessage(const MovedDown& movedDown) {
    std::cout << "DroneInstance:: received " << movedDown.messageHeader << " from Unit\n";
    pendingMessages.push(DroneMessage{ movedDown });
}

void DroneInstance::handleMessage(const MovedLeft& movedLeft) {
    std::cout << "DroneInstance:: received " << movedLeft.messageHeader << " from Unit\n";
    pendingMessages.push(DroneMessage{ movedLeft });
}

void DroneInstance::handleMessage(const MovedRight& movedRight) {
    std::cout << "DroneInstance:: received " << movedRight.messageHeader << " from Unit\n";
    pendingMessages.push(DroneMessage{ movedRight });
}

void DroneInstance::awaitMoveDone()
{
    std::cout << "DroneInstance:: await for incoming MoveDone\n";
    const auto adminId = static_cast<unsigned short>(commandingAdminId);
    const auto received = connection->receiveMoveDone(context.id, adminId);
    if(received)
    {
        std::cout << "DroneInstance:: target drone has completed: "
            << received->messageHeader
            << " task with result : "
            << resultToStr(received->result)
            << std::endl;
        pendingMessages.push(DroneMessage{ *received });
    }
    else
    {
        std::cout << "DroneInstance:: TIMEOUT while waiting for MoveDone\n";
        pendingMessages.push(DroneMessage{ MoveDone{"MOVE_DONE",
                                              context.id,
                                              adminId,
                                              TransformationResult::TIMEOUT} });
    }
    commandingAdminId = -1;
}

void DroneInstance::awaitRotateDone()
{
    std::cout << "DroneInstance:: await for incoming RotateDone\n";
    const auto adminId = static_cast<unsigned short>(commandingAdminId);
    const auto received = connection->receiveRotateDone(context.id, adminId);
    if (received)
    {
        std::cout << "DroneInstance: target drone has completed: "
            << received->messageHeader
            << " task with result : "
            << resultToStr(received->result)
            << std::endl;
        pendingMessages.push(DroneMessage{ *received });
    }
    else
    {
        std::cout << "DroneInstance:: TIMEOUT while waiting for RotateDone\n";
        pendingMessages.push(DroneMessage{ RotateDone{"ROTATE_DONE",
                                              context.id,
                                              adminId,
                                              TransformationResult::TIMEOUT} });
    }
    commandingAdminId = -1;
}

void DroneInstance::close(const std::string& cause)
{
    std::cout << "DroneInstance:: closing connection\n";
    connection->close(cause);
    shutdown = true;
}

std::optional<DroneMessage> DroneInstance::getPendingMessage() {
    if (pendingMessages.empty())
    {
        return std::nullopt;
    }
    std::cout << "DroneInstance:: get Pending Message to handle\n";
    const auto message = pendingMessages.front();
    pendingMessages.pop();
    return message;
}
