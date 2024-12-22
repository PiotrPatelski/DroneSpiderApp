#pragma once
#include "DroneConnectionProxy.hpp"
#include "UnitContext.hpp"
#include "Message.hpp"
#include <memory>
#include <vector>
#include <queue>
#include <string>
#include <optional>
#include <thread>

class DroneInstance {
public:
    DroneInstance(std::unique_ptr<DroneConnectionProxy> connection, const unsigned short instanceId);
    ~DroneInstance();

    void start();
    void close(const std::string& cause);
    void handleMessage(const MoveCommand& moveCommand);
    void handleMessage(const RotateCommand& rotateCommand);
    void handleMessage(const MoveCommandAck& moveCommandAck);
    void handleMessage(const RotateCommandAck& rotateCommandAck);
    void handleMessage(const DroneStatus& droneStatus);
    void handleMessage(const ConnectionLostInd& connectionLostInd);
    void handleMessage(const MoveUp& moveUp);
    void handleMessage(const MoveDown& moveDown);
    void handleMessage(const MoveLeft& moveLeft);
    void handleMessage(const MoveRight& moveRight);
    void handleMessage(const MovedUp& movedUp);
    void handleMessage(const MovedDown& movedDown);
    void handleMessage(const MovedLeft& movedLeft);
    void handleMessage(const MovedRight& movedRight);

    inline unsigned short getId() const { return context.id; }
    inline DroneContext getContext() const { return context; }
    std::optional<DroneMessage> getPendingMessage();
private:
    void run();
    void awaitMoveDone();
    void awaitRotateDone();
    std::string receive(const std::string& matcher);

    short commandingAdminId{ -1 };
    bool shutdown{ false };
    DroneContext context{};

    std::queue<DroneMessage> pendingMessages{};
    std::unique_ptr<DroneConnectionProxy> connection{ nullptr };

    std::thread workerThread;
};