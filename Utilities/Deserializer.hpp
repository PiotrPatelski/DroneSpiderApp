#pragma once
#include <string>
#include "Message.hpp"

EchoRequest deserializeEchoRequest(const std::string& data);

EchoCfm deserializeEchoCfm(const std::string& data);

MoveRequest deserializeMoveRequest(const std::string& data);

RotateRequest deserializeRotateRequest(const std::string& data);

StatusRequest deserializeStatusRequest(const std::string& data);

StatusInd deserializeStatusInd(const std::string& data);

ShutdownServerCommand deserializeShutdownServerCommand(const std::string& data);

ShutdownServerCfm deserializeShutdownServerCfm(const std::string& data);

MoveDone deserializeMoveDone(const std::string& data);

RotateDone deserializeRotateDone(const std::string& data);

DroneContext deserializeDroneContext(std::istringstream& iss);

DroneContext deserializeDroneContext(const std::string& data);

ServerPasswordInd deserializeServerPasswordInd(const std::string& data);

AdminLoginCfm deserializeAdminLoginCfm(const std::string& data);

MoveUp deserializeMoveUp(const std::string& data);

MoveDown deserializeMoveDown(const std::string& data);

MoveLeft deserializeMoveLeft(const std::string& data);

MoveRight deserializeMoveRight(const std::string& data);

MovedUp deserializeMovedUp(const std::string& data);

MovedDown deserializeMovedDown(const std::string& data);

MovedLeft deserializeMovedLeft(const std::string& data);

MovedRight deserializeMovedRight(const std::string& data);

ConnectionLostInd deserializeConnectionLostInd(const std::string& data);
