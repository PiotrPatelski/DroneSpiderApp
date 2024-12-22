#include "AdminInstance.hpp"
#include "UnitContext.hpp"
#include "Serializer.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>

AdminInstance::AdminInstance(std::unique_ptr<AdminConnectionProxy> connection, const unsigned short instanceId)
    : connection(std::move(connection)),
    instanceId{ instanceId } {
    std::cout << "AdminInstance:: init\n";
}

AdminInstance::~AdminInstance() {
    if (workerThread.joinable())
    {
        workerThread.join();
    }
    std::cout << "AdminInstance:: closing!\n";
}

void AdminInstance::run() {
    std::cout << "AdminInstance:: run\n";
    while (!shutdown) {
        const auto receivedMessage = connection->receiveMessage();
        if (receivedMessage)
        {
            std::cout << "AdminInstance:: received new Message\n";
            std::visit([this](auto&& message) { handleMessage(message); }, *receivedMessage);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "AdminInstance stop\n";
}

void AdminInstance::start() {
    std::cout << "AdminInstance:: start instance thread\n";
    workerThread = std::thread(&AdminInstance::run, this);
}

template <typename ClientMessage>
void AdminInstance::handleClientMessage(const ClientMessage& message) {
    std::cout << "AdminInstance:: received "
        << message.messageHeader
        << " from clientId: "
        << message.senderId
        << ", forwarding to dispatcher\n";
    pendingMessages.push(AdminMessage{ message });
}

template <typename ServerMessage>
void AdminInstance::handleServerMessage(const ServerMessage& message) {
    std::cout << "AdminInstance:: received "
        << message.messageHeader
        << " from droneId: "
        << message.senderId
        << ", forwarding to admin client\n";
    connection->sendMessage(message);
}

void AdminInstance::handleMessage(const MoveRequest& moveRequest) {
    handleClientMessage(moveRequest);
}

void AdminInstance::handleMessage(const RotateRequest& rotateRequest) {
    handleClientMessage(rotateRequest);
}

void AdminInstance::handleMessage(const StatusRequest& statusRequest) {
    handleClientMessage(statusRequest);
}

void AdminInstance::handleMessage(const ShutdownServerCommand& shutdownServerCmd) {
    handleClientMessage(shutdownServerCmd);
}

void AdminInstance::handleMessage(const ConnectionLostInd& connectionLostInd) {
    handleClientMessage(connectionLostInd);
}

void AdminInstance::handleMessage(const MoveUp& moveUp) {
    handleClientMessage(moveUp);
}

void AdminInstance::handleMessage(const MoveDown& moveDown) {
    handleClientMessage(moveDown);
}

void AdminInstance::handleMessage(const MoveLeft& moveLeft) {
    handleClientMessage(moveLeft);
}

void AdminInstance::handleMessage(const MoveRight& moveRight) {
    handleClientMessage(moveRight);
}

void AdminInstance::handleMessage(const MoveDone& moveDone) {
    handleServerMessage(moveDone);
}

void AdminInstance::handleMessage(const RotateDone& rotateDone) {
    handleServerMessage(rotateDone);
}

void AdminInstance::handleMessage(const StatusInd& statusInd) {
    handleServerMessage(statusInd);
}

void AdminInstance::handleMessage(const EchoRequest& echoRequest) {
    const auto& header = echoRequest.messageHeader;
    std::cout << "AdminInstance:: received "
        << header
        << " from serverId: "
        << echoRequest.senderId
        << ", forwarding to admin client\n";
    connection->sendMessage(EchoCfm{"ECHO_CFM", instanceId});
}

void AdminInstance::handleMessage(const MovedUp& movedUp) {
    handleServerMessage(movedUp);
}

void AdminInstance::handleMessage(const MovedDown& movedDown) {
    handleServerMessage(movedDown);
}

void AdminInstance::handleMessage(const MovedLeft& movedLeft) {
    handleServerMessage(movedLeft);
}

void AdminInstance::handleMessage(const MovedRight& movedRight) {
    handleServerMessage(movedRight);
}

void AdminInstance::close(const std::string& cause)
{
    std::cout << "AdminInstance:: closing connection\n";
    connection->close(cause);
    shutdown = true;
}

std::optional<AdminMessage> AdminInstance::getPendingMessage() {
    if (pendingMessages.empty())
    {
        return std::nullopt;
    }
    std::cout << "AdminInstance:: get Pending Message to handle\n";
    const auto message = pendingMessages.front();
    pendingMessages.pop();
    return message;
}
