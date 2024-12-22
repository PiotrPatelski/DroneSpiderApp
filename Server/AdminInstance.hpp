#pragma once
#include "AdminConnectionProxy.hpp"
#include "UnitContext.hpp"
#include "Message.hpp"
#include <memory>
#include <vector>
#include <queue>
#include <string>
#include <optional>
#include <thread>

class AdminInstance {
public:
    AdminInstance(std::unique_ptr<AdminConnectionProxy> connection, const unsigned short instanceId);
    ~AdminInstance();

    void start();
    void close(const std::string& cause);
    void handleMessage(const MoveDone& moveDone);
    void handleMessage(const RotateDone& rotateDone);
    void handleMessage(const StatusInd& statusInd);
    void handleMessage(const EchoRequest& echoRequest);
    void handleMessage(const MoveRequest& moveRequest);
    void handleMessage(const RotateRequest& rotateRequest);
    void handleMessage(const StatusRequest& statusRequest);
    void handleMessage(const ShutdownServerCommand& shutdownServerCmd);
    void handleMessage(const ConnectionLostInd& connectionLostInd);
    void handleMessage(const MoveUp& moveUp);
    void handleMessage(const MoveDown& moveDown);
    void handleMessage(const MoveLeft& moveLeft);
    void handleMessage(const MoveRight& moveRight);
    void handleMessage(const MovedUp& movedUp);
    void handleMessage(const MovedDown& movedDown);
    void handleMessage(const MovedLeft& movedLeft);
    void handleMessage(const MovedRight& movedRight);

    inline unsigned getId() const { return instanceId; }
    std::optional<AdminMessage> getPendingMessage();
private:
    void run();

    template <typename ClientMessage>
    void handleClientMessage(const ClientMessage& message);

    template <typename ServerMessage>
    void handleServerMessage(const ServerMessage& message);

    unsigned short instanceId{};
    bool shutdown{ false };

    std::queue<AdminMessage> pendingMessages{};
    std::unique_ptr<AdminConnectionProxy> connection{ nullptr };

    std::thread workerThread;
};