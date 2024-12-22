#pragma once
#include "InstanceCreator.hpp"
#include "AdminInstance.hpp"
#include "DroneInstance.hpp"
#include "ListeningConnection.hpp"
#include "Message.hpp"
#include <memory>
#include <vector>

class ConnectionManager {
public:
    ConnectionManager(std::unique_ptr<InstanceCreator>);
    ~ConnectionManager();

    void start();

private:
    void addInstanceForAttachedConnection();
    void createAndSendStatusIndToRequestingAdmin(AdminInstance& admin);
    void dispatchMessage(const MoveRequest& moveRequest);
    void dispatchMessage(const MoveDone& moveDone);
    void dispatchMessage(const RotateRequest& rotateRequest);
    void dispatchMessage(const RotateDone& rotateDone);
    void dispatchMessage(const StatusRequest& statusRequest);
    void dispatchMessage(const ShutdownServerCommand& shutdownCommand);
    void dispatchMessage(const ConnectionLostInd& ind);
    void dispatchMessage(const MoveUp& moveUp);
    void dispatchMessage(const MoveDown& moveDown);
    void dispatchMessage(const MoveLeft& moveLeft);
    void dispatchMessage(const MoveRight& moveRight);
    void dispatchMessage(const MovedUp& movedUp);
    void dispatchMessage(const MovedDown& movedDown);
    void dispatchMessage(const MovedLeft& movedLeft);
    void dispatchMessage(const MovedRight& movedRight);

    bool shutdown{ false };
    std::vector<std::unique_ptr<AdminInstance>> adminInstances{};
    std::vector<std::unique_ptr<DroneInstance>> droneInstances{};
    std::unique_ptr<ListeningConnection> listeningConnection{ nullptr };
    std::unique_ptr<InstanceCreator> instanceCreator{ nullptr };
};