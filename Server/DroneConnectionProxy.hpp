#pragma once
#include <memory>
#include <mutex>
#include <optional>
#include <variant>
#include "TcpServer.hpp"
#include "UdpServer.hpp"
#include "Message.hpp"

class DroneConnectionProxy
{
public:
	DroneConnectionProxy(std::unique_ptr<TcpServer> tcpServer, std::unique_ptr<UdpServer> udpServer);
	virtual ~DroneConnectionProxy() = default;
	using DroneMessageVariant = std::optional<
		std::variant<
		MoveCommandAck,
		RotateCommandAck,
		DroneStatus,
		ConnectionLostInd,
		MovedUp,
		MovedDown,
		MovedLeft,
		MovedRight>>;
	virtual DroneMessageVariant receiveMessage(const unsigned short instanceId);
	virtual std::optional<MoveDone> receiveMoveDone(const unsigned short instanceId, const unsigned short commandingAdminId);
	virtual std::optional<RotateDone> receiveRotateDone(const unsigned short instanceId, const unsigned short commandingAdminId);
	virtual void sendMessage(const MoveCommand& moveCommand);
	virtual void sendMessage(const RotateCommand& rotateCommand);
	virtual void sendMessage(const MoveUp& moveUp);
	virtual void sendMessage(const MoveDown& moveDown);
	virtual void sendMessage(const MoveLeft& moveLeft);
	virtual void sendMessage(const MoveRight& moveRight);
	virtual void close(const std::string& cause);
private:
	bool receivedMatches(const std::string& matcher);
	DroneMessageVariant deserializeTcp(const std::string& receivedMessage, const unsigned short instanceId);
	DroneMessageVariant deserializeUdp(const std::string& receivedMessage, const unsigned short instanceId);
	std::unique_ptr<TcpServer> tcpServer{ nullptr };
	std::unique_ptr<UdpServer> udpServer{ nullptr };
	std::mutex endpointMutex{};
};