#pragma once
#include "TcpServer.hpp"
#include "UdpServer.hpp"
#include "UnitContext.hpp"
#include "UserIdProvider.hpp"
#include <memory>
#include <vector>
#include <thread>

static constexpr unsigned short serverId{ 0 };

enum class ClientMode {
    NONE,
    ADMIN,
    DRONE
};

class ListeningConnection {
public:
    ListeningConnection();
    ~ListeningConnection();

    void start();
    void reset();
    bool foundConnection() const;
    inline ClientMode getConnectionMode() const { return clientMode; }
    inline unsigned short getConnectedClientId() const { return connectedClientId; }
    std::unique_ptr<TcpServer> releaseTcpPipe();
    std::unique_ptr<UdpServer> releaseUdpPipe();
private:
    void close();
    void run();
    void handleTcpMessage(const std::string& receivedMessage);
    void handleIncomingAdminConnection();
    void handleIncomingDroneConnection();
    inline bool isConnectionModeDefined() const { return clientMode != ClientMode::NONE; }

    bool connected{ false };
    ClientMode clientMode{ ClientMode::NONE };
    UserIdProvider userIdProvider{};
    unsigned short connectedClientId{ 0 };
    std::unique_ptr<TcpServer> tcpServer{ nullptr };
    std::unique_ptr<UdpServer> udpServer{ nullptr };
    std::thread listenThread;
};