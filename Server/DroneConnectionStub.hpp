#pragma once
#include <memory>
#include <mutex>
#include <optional>
#include <variant>
#include <deque>
#include "TcpServer.hpp"
#include "UdpServer.hpp"
#include "Message.hpp"
#include "DroneConnectionProxy.hpp"

class DroneConnectionStub : public DroneConnectionProxy
{
public:
	DroneConnectionStub(std::unique_ptr<TcpServer> tcpServer, std::unique_ptr<UdpServer> udpServer);

	using DroneResponse = std::variant<
		MoveCommandAck,
		RotateCommandAck,
		DroneStatus,
		ConnectionLostInd,
		MovedUp,
		MovedDown,
		MovedLeft,
		MovedRight>;

	using DroneMessageVariant = std::optional<DroneResponse>;
	
	DroneMessageVariant receiveMessage(const unsigned short instanceId);
	std::optional<MoveDone> receiveMoveDone(const unsigned short instanceId, const unsigned short commandingAdminId);
	std::optional<RotateDone> receiveRotateDone(const unsigned short instanceId, const unsigned short commandingAdminId);
	void sendMessage(const MoveCommand& moveCommand);
	void sendMessage(const RotateCommand& rotateCommand);
	void sendMessage(const MoveUp& moveUp);
	void sendMessage(const MoveDown& moveDown);
	void sendMessage(const MoveLeft& moveLeft);
	void sendMessage(const MoveRight& moveRight);
	void close(const std::string& cause);
private:
	void startDroneStatusThread();
	std::deque< DroneResponse> messageQueue{};
	DroneContext context{99, UnitStatus::IDLE, {0.f, 0.f}, {0.f} };
	std::thread statusThread;
	std::mutex endpointMutex{};
	bool shutdown{ false };
};