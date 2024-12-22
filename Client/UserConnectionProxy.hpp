#pragma once
#include <memory>
#include <mutex>
#include <optional>
#include <variant>
#include "TcpClient.hpp"
#include "UdpClient.hpp"
#include "Message.hpp"

class UserConnectionProxy
{
public:
	UserConnectionProxy(std::unique_ptr<TcpClient> tcpServer, std::unique_ptr<UdpClient> udpServer);

	using UserMessageVariant = std::optional<
		std::variant<
		EchoCfm,
		StatusInd,
		MoveDone,
		RotateDone,
		ShutdownServerCfm,
		ConnectionLostInd>>;
	UserMessageVariant receiveMessage();
	UserMessageVariant receiveUdpMessage();
	void sendMessage(const MoveRequest& moveRequest);
	void sendMessage(const RotateRequest& rotateRequest);
	void sendMessage(const StatusRequest& statusRequest);
	void sendMessage(const EchoRequest& echoRequest);
	void sendMessage(const ShutdownServerCommand& shutdownCommand);
	void sendMessage(const MoveUp& moveUp);
	void sendMessage(const MoveDown& moveDown);
	void sendMessage(const MoveLeft& moveLeft);
	void sendMessage(const MoveRight& moveRight);
	void close(const std::string& cause);
private:
	UserMessageVariant deserializeTcp(const std::string& receivedMessage);
	UserMessageVariant deserializeUdp(const std::string& receivedMessage);
	std::unique_ptr<TcpClient> tcpClient{ nullptr };
	std::unique_ptr<UdpClient> udpClient{ nullptr };
	std::mutex endpointMutex{};
};