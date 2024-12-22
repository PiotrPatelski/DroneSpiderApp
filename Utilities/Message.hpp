#pragma once
#include <string>
#include <variant>
#include <vector>
#include "UnitContext.hpp"

enum class TransformationResult
{
	FINISHED,
	INTERRUPED,
	TIMEOUT,
	UNDEFINED
};

TransformationResult strToResult(const std::string& result);

std::string resultToStr(const TransformationResult result);

std::string getMessageHeader(const std::string& data);

struct Message {
public:
	std::string messageHeader{ "" };
	unsigned senderId{ 0 };
};

struct AdminLoginCfm : public Message {
	unsigned short userId{ 0 };
};

struct ServerPasswordInd : public Message {
	std::string password{ "" };
};

struct EchoRequest : public Message {};

struct EchoCfm : public Message {};

struct MoveRequest : public Message {
	std::vector<unsigned short> receiverIds{};
	int xPosition{ 0 };
	int yPosition{ 0 };
};

struct MoveCommand : public Message{
	unsigned short receiverId{};
	int xPosition{ 0 };
	int yPosition{ 0 };
};

struct RotateRequest : public Message {
	std::vector<unsigned short> receiverIds{};
	int angle{ 0 };
};

struct RotateCommand : public Message {
	unsigned short receiverId{};
	int angle{ 0 };
};

struct StatusRequest : public Message {

};

struct StatusInd : public Message {
	unsigned short receiverId{};
	std::vector<DroneContext> drones;
};

struct ShutdownServerCommand : public Message {

};

struct ShutdownServerCfm : public Message {

};

struct ConnectionLostInd : public Message {

};

struct MoveDone : public Message {
	unsigned short receiverId{ 0 };
	TransformationResult result{ TransformationResult::UNDEFINED };
};

struct RotateDone : public Message {
	unsigned short receiverId{ 0 };
	TransformationResult result{ TransformationResult::UNDEFINED };
};

struct MoveUp : public Message 
{
	unsigned short receiverId{ 0 };
};

struct MoveDown : public Message
{
	unsigned short receiverId{ 0 };
};

struct MoveLeft : public Message 
{
	unsigned short receiverId{ 0 };
};

struct MoveRight : public Message
{
	unsigned short receiverId{ 0 };
};

struct MovedUp : public Message 
{ 
	unsigned short receiverId{ 0 }; 
};

struct MovedDown : public Message 
{ 
	unsigned short receiverId{ 0 }; 
};

struct MovedLeft : public Message 
{ 
	unsigned short receiverId{ 0 }; 
};

struct MovedRight : public Message 
{ 
	unsigned short receiverId{ 0 }; 
};

struct AdminMessage {
	std::variant<
		MoveRequest, 
		RotateRequest, 
		StatusRequest, 
		ShutdownServerCommand, 
		ConnectionLostInd,
		MoveUp,
		MoveDown,
		MoveLeft,
		MoveRight> data;
};

struct MoveCommandAck : public Message {};

struct RotateCommandAck : public Message {};

struct DroneStatus : public Message 
{
	DroneContext data{};
};

struct DroneMessage {
	std::variant<
		MoveDone,
		RotateDone,
		MovedUp,
		MovedDown,
		MovedLeft,
		MovedRight,
		ConnectionLostInd> data;
};
