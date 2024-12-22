#include "Deserializer.hpp"
#include <sstream>

std::vector<unsigned short> getReceiverIdsFromStream(std::istringstream& inputStream)
{
    std::size_t vectorSize;
    inputStream >> vectorSize;
    std::vector<unsigned short> result;
    for (std::size_t i = 0; i < vectorSize; ++i) {
        unsigned short id;
        inputStream >> id;
        result.push_back(id);
    }
    return result;
}

EchoRequest deserializeEchoRequest(const std::string& data) {
    EchoRequest msg;
    std::istringstream iss(data);

    iss >> msg.messageHeader
        >> msg.senderId;

    return msg;
}

EchoCfm deserializeEchoCfm(const std::string& data) {
    EchoCfm msg;
    std::istringstream iss(data);

    iss >> msg.messageHeader
        >> msg.senderId;

    return msg;
}

MoveRequest deserializeMoveRequest(const std::string& data) {
    MoveRequest msg;
    std::istringstream iss(data);

    iss >> msg.messageHeader;
    iss >> msg.senderId;
    msg.receiverIds = getReceiverIdsFromStream(iss);
    iss >> msg.xPosition
        >> msg.yPosition;

    return msg;
}

RotateRequest deserializeRotateRequest(const std::string& data) {
    RotateRequest msg;
    std::istringstream iss(data);

    iss >> msg.messageHeader;
    iss >> msg.senderId;
    msg.receiverIds = getReceiverIdsFromStream(iss);
    iss >> msg.angle;

    return msg;
}

ShutdownServerCommand deserializeShutdownServerCommand(const std::string& data) {
    ShutdownServerCommand msg;
    std::istringstream iss(data);

    iss >> msg.messageHeader
        >> msg.senderId;

    return msg;
}

ShutdownServerCfm deserializeShutdownServerCfm(const std::string& data) {
    ShutdownServerCfm msg;
    std::istringstream iss(data);

    iss >> msg.messageHeader
        >> msg.senderId;

    return msg;
}

MoveDone deserializeMoveDone(const std::string& data) {
    MoveDone msg;
    std::istringstream iss(data);
    std::string resultStr{""};
    iss >> msg.messageHeader
        >> msg.senderId
        >> msg.receiverId
        >> resultStr;
    msg.result = strToResult(resultStr);

    return msg;
}

RotateDone deserializeRotateDone(const std::string& data) {
    RotateDone msg;
    std::istringstream iss(data);
    std::string resultStr{ "" };
    iss >> msg.messageHeader
        >> msg.senderId
        >> msg.receiverId
        >> resultStr;
    msg.result = strToResult(resultStr);

    return msg;
}

StatusInd deserializeStatusInd(const std::string& data) {
    StatusInd statusInd;
    std::istringstream iss(data);

    iss >> statusInd.messageHeader;
    iss >> statusInd.senderId;
    iss >> statusInd.receiverId;
    std::size_t numDrones;
    iss >> numDrones;

    for (std::size_t i = 0; i < numDrones; ++i) {
        statusInd.drones.push_back(deserializeDroneContext(iss));
    }

    return statusInd;
}

DroneContext deserializeDroneContext(std::istringstream& iss) {
    DroneContext drone;
    unsigned short status{0};

    iss >> drone.id
        >> status
        >> drone.position.x
        >> drone.position.y
        >> drone.orientation;
    drone.status = static_cast<UnitStatus>(status);

    return drone;
}

DroneContext deserializeDroneContext(const std::string& data) {
    DroneContext drone;
    unsigned short status{ 0 };
    std::istringstream iss(data);
    iss >> drone.id
        >> status
        >> drone.position.x
        >> drone.position.y
        >> drone.orientation;
    drone.status = static_cast<UnitStatus>(status);

    return drone;
}

ServerPasswordInd deserializeServerPasswordInd(const std::string& data) {
    ServerPasswordInd msg;
    std::istringstream iss(data);

    iss >> msg.messageHeader;
    iss >> msg.senderId;
    iss >> msg.password;

    return msg;
}

 AdminLoginCfm deserializeAdminLoginCfm(const std::string& data) {
    AdminLoginCfm msg;
    std::istringstream iss(data);

    iss >> msg.messageHeader;
    iss >> msg.senderId;
    iss >> msg.userId;

    return msg;
}

MoveUp deserializeMoveUp(const std::string& data)
{
    MoveUp msg;
    std::istringstream iss(data);

    iss >> msg.messageHeader;
    iss >> msg.senderId;
    iss >> msg.receiverId;

    return msg;
}

MoveDown deserializeMoveDown(const std::string& data)
{
    MoveDown msg;
    std::istringstream iss(data);

    iss >> msg.messageHeader;
    iss >> msg.senderId;
    iss >> msg.receiverId;

    return msg;
}

MoveLeft deserializeMoveLeft(const std::string& data)
{
    MoveLeft msg;
    std::istringstream iss(data);

    iss >> msg.messageHeader;
    iss >> msg.senderId;
    iss >> msg.receiverId;

    return msg;
}

MoveRight deserializeMoveRight(const std::string& data)
{
    MoveRight msg;
    std::istringstream iss(data);

    iss >> msg.messageHeader;
    iss >> msg.senderId;
    iss >> msg.receiverId;

    return msg;
}

MovedUp deserializeMovedUp(const std::string& data)
{
    MovedUp msg;
    std::istringstream iss(data);

    iss >> msg.messageHeader;
    iss >> msg.senderId;
    iss >> msg.receiverId;

    return msg;
}

MovedDown deserializeMovedDown(const std::string& data)
{
    MovedDown msg;
    std::istringstream iss(data);

    iss >> msg.messageHeader;
    iss >> msg.senderId;
    iss >> msg.receiverId;

    return msg;
}

MovedLeft deserializeMovedLeft(const std::string& data)
{
    MovedLeft msg;
    std::istringstream iss(data);

    iss >> msg.messageHeader;
    iss >> msg.senderId;
    iss >> msg.receiverId;

    return msg;
}

MovedRight deserializeMovedRight(const std::string& data)
{
    MovedRight msg;
    std::istringstream iss(data);

    iss >> msg.messageHeader;
    iss >> msg.senderId;
    iss >> msg.receiverId;

    return msg;
}

StatusRequest deserializeStatusRequest(const std::string& data) {
    StatusRequest msg;

    std::istringstream iss(data);

    iss >> msg.messageHeader;
    iss >> msg.senderId;

    return msg;
}

ConnectionLostInd deserializeConnectionLostInd(const std::string& data) {
    ConnectionLostInd msg;

    std::istringstream iss(data);

    iss >> msg.messageHeader;
    iss >> msg.senderId;

    return msg;
}
