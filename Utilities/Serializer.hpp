#pragma once
#include <string>
#include "Message.hpp"

std::string serialize(const EchoRequest& echoRequest);

std::string serialize(const EchoCfm& echoCfm);

std::string serialize(const MoveDone& moveDone);

std::string serialize(const RotateDone& rotateDone);

std::string serialize(const StatusRequest& statusRequest);

std::string serialize(const StatusInd& statusInd);

std::string serialize(const DroneContext& drone);

std::string serialize(const MoveCommand& moveCommand);

std::string serialize(const RotateCommand& rotateCommand);

std::string serialize(const AdminLoginCfm& loginCfm);

std::string serialize(const ServerPasswordInd& passwordInd);

std::string serialize(const MoveRequest& moveRequest);

std::string serialize(const RotateRequest& rotateRequest);

std::string serialize(const ShutdownServerCommand& shutdownCommand);

std::string serialize(const ShutdownServerCfm& shutdownServerCfm);

std::string serialize(const MoveUp& moveUp);

std::string serialize(const MoveDown& moveDown);

std::string serialize(const MoveLeft& moveLeft);

std::string serialize(const MoveRight& moveRight);

std::string serialize(const MovedUp& movedUp);

std::string serialize(const MovedDown& movedDown);

std::string serialize(const MovedLeft& movedLeft);

std::string serialize(const MovedRight& movedRight);

std::string serialize(const ConnectionLostInd& ConnectionLostInd);

std::string serialize(const std::vector<unsigned short>& ids);