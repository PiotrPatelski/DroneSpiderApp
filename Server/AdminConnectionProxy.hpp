#pragma once
#include <memory>
#include <mutex>
#include <optional>
#include <variant>
#include "TcpServer.hpp"
#include "UdpServer.hpp"
#include "Message.hpp"

class AdminConnectionProxy
{
public:
	AdminConnectionProxy(std::unique_ptr<TcpServer> tcpServer, std::unique_ptr<UdpServer> udpServer);

	using AdminMessageVariant = std::optional<
		std::variant<
		MoveRequest,
		RotateRequest,
		StatusRequest,
		EchoRequest,
		ShutdownServerCommand,
		ConnectionLostInd,
		MoveUp,
		MoveDown,
		MoveLeft,
		MoveRight>>;
	AdminMessageVariant receiveMessage();
	void sendMessage(const MoveDone& moveDone);
	void sendMessage(const RotateDone& rotateDone);
	void sendMessage(const StatusInd& statusInd);
	void sendMessage(const EchoCfm& echoCfm);
	void sendMessage(const MovedUp& movedUp);
	void sendMessage(const MovedDown& movedDown);
	void sendMessage(const MovedLeft& movedLeft);
	void sendMessage(const MovedRight& movedRight);
	void close(const std::string& cause);
private:
	AdminMessageVariant deserializeTcp(const std::string& receivedMessage);
	AdminMessageVariant deserializeUdp(const std::string& receivedMessage);
	std::unique_ptr<TcpServer> tcpServer{ nullptr };
	std::unique_ptr<UdpServer> udpServer{ nullptr };
	std::mutex endpointMutex{};
};