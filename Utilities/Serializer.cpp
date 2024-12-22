#include "Serializer.hpp"
#include <sstream>
#include <iostream>

std::string serialize(const EchoRequest& echoRequest) {
    std::ostringstream oss;

    oss << echoRequest.messageHeader << ' '
        << echoRequest.senderId << ' ';

    return oss.str();
}

std::string serialize(const EchoCfm& echoCfm) {
    std::ostringstream oss;

    oss << echoCfm.messageHeader << ' '
        << echoCfm.senderId << ' ';

    return oss.str();
}

std::string serialize(const MoveDone& moveDone) {
    std::ostringstream oss;

    oss << moveDone.messageHeader << ' '
        << moveDone.senderId << ' '
        << moveDone.receiverId << ' '
        << resultToStr(moveDone.result);

    return oss.str();
}

std::string serialize(const RotateDone& rotateDone) {
    std::ostringstream oss;

    oss << rotateDone.messageHeader << ' '
        << rotateDone.senderId << ' '
        << rotateDone.receiverId << ' '
        << resultToStr(rotateDone.result);

    return oss.str();
}

std::string serialize(const StatusRequest& statusRequest) {
    std::ostringstream oss;

    oss << statusRequest.messageHeader << ' '
        << statusRequest.senderId << ' ';

    return oss.str();
}

std::string serialize(const StatusInd& statusInd) {
    std::ostringstream oss;
    oss << statusInd.messageHeader << ' '
        << statusInd.senderId << ' '
        << statusInd.receiverId << ' '
        << statusInd.drones.size() << ' ';
    std::cout << "serialize StatusInd: "
        << "\n{header: " << statusInd.messageHeader
        << "\n senderId: " << statusInd.senderId
        << "\n receiverId: " << statusInd.receiverId
        << "\n drones.size: " << statusInd.drones.size()
        << "}\n";
    for (const auto& droneContext : statusInd.drones) {
        oss << serialize(droneContext) << ' ';
    }

    return oss.str();
}

std::string serialize(const DroneContext& drone) {
    std::ostringstream oss;
    std::cout << "serialize DroneContext: "
        << "\n{id: " << drone.id
        << "\n status: " << drone.status
        << "\n x: " << drone.position.x
        << "\n y: " << drone.position.y
        << "\n orientation: " << drone.orientation
        << "}\n";
    oss << drone.id << ' '
        << static_cast<unsigned short>(drone.status) << ' '
        << drone.position.x << " " << drone.position.y << ' '
        << drone.orientation;
    return oss.str();
}

std::string serialize(const MoveCommand& moveCommand) {
    std::ostringstream oss;

    oss << moveCommand.messageHeader << ' '
        << moveCommand.senderId << ' '
        << moveCommand.receiverId << ' '
        << moveCommand.xPosition << ' '
        << moveCommand.yPosition;

    return oss.str();
}

std::string serialize(const RotateCommand& rotateCommand) {
    std::ostringstream oss;

    oss << rotateCommand.messageHeader << ' '
        << rotateCommand.senderId << ' '
        << rotateCommand.receiverId << ' '
        << rotateCommand.angle;

    return oss.str();
}

std::string serialize(const AdminLoginCfm& loginCfm) {
    std::ostringstream oss;

    oss << loginCfm.messageHeader << ' '
        << loginCfm.senderId << ' '
        << loginCfm.userId;

    return oss.str();
}

std::string serialize(const ServerPasswordInd& passwordInd) {
    std::ostringstream oss;

    oss << passwordInd.messageHeader << ' '
        << passwordInd.senderId << ' '
        << passwordInd.password;

    return oss.str();
}

std::string serialize(const std::vector<unsigned short>& ids)
{
    std::ostringstream oss;
    oss << ids.size() << ' ';
    for (const auto& id : ids) {
        oss << id << ' ';
    }
    return oss.str();
}

std::string serialize(const MoveRequest& moveRequest) {
    std::ostringstream oss;

    oss << moveRequest.messageHeader << ' '
        << moveRequest.senderId << ' ';
    oss << serialize(moveRequest.receiverIds);
    oss << moveRequest.xPosition << ' '
        << moveRequest.yPosition;

    return oss.str();
}

std::string serialize(const RotateRequest& rotateRequest) {
    std::ostringstream oss;

    oss << rotateRequest.messageHeader << ' '
        << rotateRequest.senderId << ' ';
    oss << serialize(rotateRequest.receiverIds);
    oss << rotateRequest.angle;

    return oss.str();
}

std::string serialize(const ShutdownServerCommand& shutdownCommand) {
    std::ostringstream oss;

    oss << shutdownCommand.messageHeader << ' '
        << shutdownCommand.senderId << ' ';

    return oss.str();
}

std::string serialize(const ShutdownServerCfm& shutdownServerCfm) {
    std::ostringstream oss;

    oss << shutdownServerCfm.messageHeader << ' '
        << shutdownServerCfm.senderId << ' ';

    return oss.str();
}

std::string serialize(const MoveUp& moveUp) {
    std::ostringstream oss;

    oss << moveUp.messageHeader << ' '
        << moveUp.senderId << ' '
        << moveUp.receiverId;

    return oss.str();
}

std::string serialize(const MoveDown& moveDown) {
    std::ostringstream oss;

    oss << moveDown.messageHeader << ' '
        << moveDown.senderId << ' '
        << moveDown.receiverId;

    return oss.str();
}

std::string serialize(const MoveLeft& moveLeft) {
    std::ostringstream oss;

    oss << moveLeft.messageHeader << ' '
        << moveLeft.senderId << ' '
        << moveLeft.receiverId;

    return oss.str();
}

std::string serialize(const MoveRight& moveRight) {
    std::ostringstream oss;

    oss << moveRight.messageHeader << ' '
        << moveRight.senderId << ' '
        << moveRight.receiverId;

    return oss.str();
}

std::string serialize(const MovedUp& movedUp) {
    std::ostringstream oss;

    oss << movedUp.messageHeader << ' '
        << movedUp.senderId << ' '
        << movedUp.receiverId;

    return oss.str();
}

std::string serialize(const MovedDown& movedDown) {
    std::ostringstream oss;

    oss << movedDown.messageHeader << ' '
        << movedDown.senderId << ' '
        << movedDown.receiverId;

    return oss.str();
}

std::string serialize(const MovedLeft& movedLeft) {
    std::ostringstream oss;

    oss << movedLeft.messageHeader << ' '
        << movedLeft.senderId << ' '
        << movedLeft.receiverId;

    return oss.str();
}

std::string serialize(const MovedRight& movedRight) {
    std::ostringstream oss;

    oss << movedRight.messageHeader << ' '
        << movedRight.senderId << ' '
        << movedRight.receiverId;

    return oss.str();
}

std::string serialize(const ConnectionLostInd& ConnectionLostInd) {
    std::ostringstream oss;

    oss << ConnectionLostInd.messageHeader << ' '
        << ConnectionLostInd.senderId << ' ';

    return oss.str();
}
