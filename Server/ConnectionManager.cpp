#include "ConnectionManager.hpp"
#include <iostream>

ConnectionManager::ConnectionManager(std::unique_ptr<InstanceCreator> instanceCreator)
    : instanceCreator(std::move(instanceCreator)),
    listeningConnection{ instanceCreator->createListeningConnection()}
{
    std::cout << "ConnectionManager:: init\n";
    listeningConnection->start();

    //TODO REMOVE STUB CODE
    std::cout << "ConnectionManager:: createDroneStub\n";
    auto droneStub = this->instanceCreator->createDroneStub(*listeningConnection);
    droneStub->start();
    droneInstances.push_back(std::move(droneStub));
    listeningConnection->reset();
    //
}

ConnectionManager::~ConnectionManager() {
    std::cout << "ConnectionManager:: closing\n";
}

void ConnectionManager::start() {
    std::cout << "ConnectionManager:: start\n";
    while (!shutdown) {
        if (listeningConnection->foundConnection())
        {
            std::cout << "ConnectionManager:: connection is ready\n";
            addInstanceForAttachedConnection();
        }
        for (const auto& admin : adminInstances)
        {
            const auto receivedMessage = admin->getPendingMessage();
            if (receivedMessage)
            {
                std::cout << "ConnectionManager:: got pending message from Admin Instance\n";
                std::visit([this](auto&& message) { dispatchMessage(message); }, receivedMessage->data);
            }
        }
        for (const auto& drone : droneInstances)
        {
            const auto receivedMessage = drone->getPendingMessage();
            if (receivedMessage)
            {
                std::cout << "ConnectionManager:: got pending message from Drone Instance\n";
                std::visit([this](auto&& message) { dispatchMessage(message); }, receivedMessage->data);
            }
        }
    }
    std::cout << "Manager shutdown" << std::endl;
}

void ConnectionManager::addInstanceForAttachedConnection(){
    const auto connectionMode = listeningConnection->getConnectionMode();
    if (connectionMode == ClientMode::ADMIN)
    {
        std::cout << "ConnectionManager:: creating new Admin Instance\n";
        auto admin = instanceCreator->createAdminInstance(*listeningConnection);
        admin->start();
        adminInstances.push_back(std::move(admin));
        listeningConnection->reset();
    }
    else if (connectionMode == ClientMode::DRONE)
    {
        std::cout << "ConnectionManager:: creating new Drone Instance\n";
        auto drone = instanceCreator->createDroneInstance(*listeningConnection);
        drone->start();
        droneInstances.push_back(std::move(drone));
        listeningConnection->reset();
    }
    else
    {
        std::cerr << "ConnectionManager::addInstanceForAttachedConnection::ERROR: listeningConnection->getConnectionMode() returned unhandled mode!" << std::endl;
    }
}

void ConnectionManager::dispatchMessage(const MoveRequest& moveRequest) {
    std::cout << "ConnectionManager:: dispatch MoveRequest\n";
    for (const auto receiverId : moveRequest.receiverIds)
    {
        std::cout << "ConnectionManager:: searching for droneId: "<< receiverId << std::endl;
        auto drone = std::find_if(droneInstances.begin(), droneInstances.end(),
            [receiverId = receiverId](const auto& connection) 
            {
                std::cout << "ConnectionManager:: receiverId: " << receiverId
                    << " matching against droneInstanceId: " << connection->getId() << std::endl;
                return connection->getId() == receiverId; 
            });
        if (drone == droneInstances.end())
        {
            std::cout << "ConnectionManager:: no droneInstance could be found with id: " << receiverId << std::endl;
            continue;
        }
        std::cout << "ConnectionManager:: found droneInstance with id: " << receiverId << std::endl;
        (*drone)->handleMessage(MoveCommand{ "MOVE_COMMAND", moveRequest.senderId, receiverId, moveRequest.xPosition, moveRequest.yPosition });
    }
    std::cout << "ConnectionManager:: dispatch MoveRequest finished\n";
}

void ConnectionManager::dispatchMessage(const RotateRequest& rotateRequest) {
    std::cout << "ConnectionManager:: dispatch RotateRequest\n";
    for (const auto receiverId : rotateRequest.receiverIds)
    {
        auto drone = std::find_if(droneInstances.begin(), droneInstances.end(),
            [receiverId = receiverId](const auto& connection) {return connection->getId() == receiverId; });
        if (drone == droneInstances.end())
        {
            continue;
        }
        (*drone)->handleMessage(RotateCommand{ "ROTATE_COMMAND", rotateRequest.senderId, receiverId, rotateRequest.angle});
    }
}

void ConnectionManager::dispatchMessage(const StatusRequest& statusRequest) {
    std::cout << "ConnectionManager:: dispatch StatusRequest\n";
    auto sender = std::find_if(adminInstances.begin(), adminInstances.end(),
        [senderId = statusRequest.senderId](const auto& connection) {return connection->getId() == senderId; });
    if (sender == adminInstances.end())
    {
        return;
    }
    std::cout << "ConnectionManager: preparing STATUS_IND for adminId: "
        << statusRequest.senderId << std::endl;
    createAndSendStatusIndToRequestingAdmin(*(*sender));
}

void ConnectionManager::dispatchMessage(const ShutdownServerCommand& shutdownCommand) {
    std::cout << "ConnectionManager:: dispatch ShutdownServerCommand\n";
    auto sender = std::find_if(adminInstances.begin(), adminInstances.end(),
        [senderId = shutdownCommand.senderId](const auto& connection) {return connection->getId() == senderId; });
    if (sender == adminInstances.end())
    {
        return;
    }
    (*sender)->close("ADMIN_INITIATED_SHUTDOWN_CFM");
    std::cout << "Server shutdown due to : "
        << shutdownCommand.messageHeader
        << " received from clientId: "
        << shutdownCommand.senderId << std::endl;
    shutdown = true;
}

void ConnectionManager::dispatchMessage(const ConnectionLostInd& ind) {
    std::cout << "ConnectionManager:: dispatch ConnectionLostInd\n";
    std::cout << "Closing connection due to : "
        << ind.messageHeader
        << " received from clientId: "
        << ind.senderId << std::endl;
    const auto foundAdminWithMatchingId = std::find_if(adminInstances.begin(), adminInstances.end(),
        [adminId = ind.senderId](const auto& connection) {return connection->getId() == adminId; });
    if (foundAdminWithMatchingId != adminInstances.end())
    {
        (*foundAdminWithMatchingId)->close("CONNECTION_LOST");
        return;
    }
    const auto foundDroneWithMatchingId = std::find_if(droneInstances.begin(), droneInstances.end(),
        [droneId = ind.senderId](const auto& connection) {return connection->getId() == droneId; });
    if (foundDroneWithMatchingId != droneInstances.end())
    {
        (*foundDroneWithMatchingId)->close("CONNECTION_LOST");
    }
  
}

void ConnectionManager::dispatchMessage(const MoveDone& moveDone) {
    std::cout << "ConnectionManager:: dispatch MoveDone\n";
    auto foundAdminWithMatchingId = std::find_if(adminInstances.begin(), adminInstances.end(),
        [commandingAdminId = moveDone.receiverId](const auto& connection) {return connection->getId() == commandingAdminId; });
    if (foundAdminWithMatchingId != adminInstances.end())
    {
        (*foundAdminWithMatchingId)->handleMessage(moveDone);
    }
}

void ConnectionManager::dispatchMessage(const RotateDone& rotateDone) {
    std::cout << "ConnectionManager:: dispatch RotateDone\n";
    auto foundAdminWithMatchingId = std::find_if(adminInstances.begin(), adminInstances.end(),
        [commandingAdminId = rotateDone.receiverId](const auto& connection) {return connection->getId() == commandingAdminId; });
    if (foundAdminWithMatchingId != adminInstances.end())
    {
        (*foundAdminWithMatchingId)->handleMessage(rotateDone);
    }
}

void ConnectionManager::dispatchMessage(const MoveUp& moveUp) {
    std::cout << "ConnectionManager:: dispatch MoveUp\n";
    const auto foundDroneWithMatchingId = std::find_if(droneInstances.begin(), droneInstances.end(),
        [droneId = moveUp.receiverId](const auto& connection) {return connection->getId() == droneId; });
    if (foundDroneWithMatchingId != droneInstances.end())
    {
        (*foundDroneWithMatchingId)->handleMessage(moveUp);
    }
}

void ConnectionManager::dispatchMessage(const MoveDown& moveDown) {
    std::cout << "ConnectionManager:: dispatch MoveDown\n";
    const auto foundDroneWithMatchingId = std::find_if(droneInstances.begin(), droneInstances.end(),
        [droneId = moveDown.receiverId](const auto& connection) {return connection->getId() == droneId; });
    if (foundDroneWithMatchingId != droneInstances.end())
    {
        (*foundDroneWithMatchingId)->handleMessage(moveDown);
    }
}

void ConnectionManager::dispatchMessage(const MoveLeft& moveLeft) {
    std::cout << "ConnectionManager:: dispatch MoveLeft\n";
    const auto foundDroneWithMatchingId = std::find_if(droneInstances.begin(), droneInstances.end(),
        [droneId = moveLeft.receiverId](const auto& connection) {return connection->getId() == droneId; });
    if (foundDroneWithMatchingId != droneInstances.end())
    {
        (*foundDroneWithMatchingId)->handleMessage(moveLeft);
    }
}

void ConnectionManager::dispatchMessage(const MoveRight& moveRight) {
    std::cout << "ConnectionManager:: dispatch MoveRight\n";
    const auto foundDroneWithMatchingId = std::find_if(droneInstances.begin(), droneInstances.end(),
        [droneId = moveRight.receiverId](const auto& connection) {return connection->getId() == droneId; });
    if (foundDroneWithMatchingId != droneInstances.end())
    {
        (*foundDroneWithMatchingId)->handleMessage(moveRight);
    }
}

void ConnectionManager::dispatchMessage(const MovedUp& movedUp) {
    std::cout << "ConnectionManager:: dispatch MovedUp\n";
    auto foundAdminWithMatchingId = std::find_if(adminInstances.begin(), adminInstances.end(),
        [commandingAdminId = movedUp.receiverId](const auto& connection) {return connection->getId() == commandingAdminId; });
    if (foundAdminWithMatchingId != adminInstances.end())
    {
        (*foundAdminWithMatchingId)->handleMessage(movedUp);
    }
}

void ConnectionManager::dispatchMessage(const MovedDown& movedDown) {
    std::cout << "ConnectionManager:: dispatch MovedDown\n";
    auto foundAdminWithMatchingId = std::find_if(adminInstances.begin(), adminInstances.end(),
        [commandingAdminId = movedDown.receiverId](const auto& connection) {return connection->getId() == commandingAdminId; });
    if (foundAdminWithMatchingId != adminInstances.end())
    {
        (*foundAdminWithMatchingId)->handleMessage(movedDown);
    }
}

void ConnectionManager::dispatchMessage(const MovedLeft& movedLeft) {
    std::cout << "ConnectionManager:: dispatch MovedLeft\n";
    auto foundAdminWithMatchingId = std::find_if(adminInstances.begin(), adminInstances.end(),
        [commandingAdminId = movedLeft.receiverId](const auto& connection) {return connection->getId() == commandingAdminId; });
    if (foundAdminWithMatchingId != adminInstances.end())
    {
        (*foundAdminWithMatchingId)->handleMessage(movedLeft);
    }
}

void ConnectionManager::dispatchMessage(const MovedRight& movedRight) {
    std::cout << "ConnectionManager:: dispatch MovedRight\n";
    auto foundAdminWithMatchingId = std::find_if(adminInstances.begin(), adminInstances.end(),
        [commandingAdminId = movedRight.receiverId](const auto& connection) {return connection->getId() == commandingAdminId; });
    if (foundAdminWithMatchingId != adminInstances.end())
    {
        (*foundAdminWithMatchingId)->handleMessage(movedRight);
    }
}

void ConnectionManager::createAndSendStatusIndToRequestingAdmin(AdminInstance& admin)
{
    std::cout << "ConnectionManager:: creating drones status to be sent to Admin Instance\n";
    StatusInd statusInd{ "STATUS_IND", serverId, admin.getId(), {} };
    for (const auto& drone : droneInstances)
    {
        const auto context = drone->getContext();
        std::cout << "ConnectionManager:: adding Context:\n{id: "<<context.id
            <<", status: "<<context.status
            <<", position: {" << context.position.x << context.position.y
            <<"}, orientation: "<< context.orientation
            <<"}\n   of Drone id : "<< drone->getId()<<std::endl;
        statusInd.drones.push_back(drone->getContext());
    }
    admin.handleMessage(statusInd);
}