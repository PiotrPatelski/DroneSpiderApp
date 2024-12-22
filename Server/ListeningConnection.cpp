#include "ListeningConnection.hpp"
#include "TcpServer.hpp"
#include "UdpServer.hpp"
#include "Message.hpp"
#include "Serializer.hpp"
#include "Deserializer.hpp"
#include <string>
#include <iostream>
#include <chrono>
#include <sstream>

static constexpr unsigned short tcpPort{ 27015 };
static constexpr unsigned short udpPort{ 27016 };
static constexpr unsigned short amountOfAllowedLoginAttempts{ 3 };
static constexpr inline std::string getServerPassword(){ return "admin"; }

static inline bool passwordMatches(const std::string& input)
{
    return getServerPassword() == input;
}

ListeningConnection::ListeningConnection()
    : tcpServer(std::make_unique<TcpServer>()),
    udpServer(std::make_unique<UdpServer>()) {
    std::cout << "ListeningConnection:: init\n";
}

ListeningConnection::~ListeningConnection() {
    close();
    if (listenThread.joinable())
    {
        listenThread.join();
    }
    std::cout << "ListeningConnection:: closing\n";
}

void ListeningConnection::start() {
    std::cout << "ListeningConnection:: start new thread\n";
    listenThread = std::thread(&ListeningConnection::run, this);
}

void ListeningConnection::run() {
    std::cout << "ListeningConnection:: run\n";
    tcpServer->addService(AF_INET);
    tcpServer->awaitForConnection("127.0.0.1", tcpPort);
    while (!connected)
    {
        const auto receivedTcpMessage = tcpServer->receiveMessage();
        if (receivedTcpMessage != "TCP_RECV_EMPTY") {
            std::cout << "ListeningConnection:: received TCP message\n";
            handleTcpMessage(receivedTcpMessage);
            continue;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }   
}

void ListeningConnection::handleTcpMessage(const std::string& receivedMessage) {
    std::cout << "ListeningConnection:: handle message: " << receivedMessage<<std::endl;
    if (receivedMessage == "ADMIN_LOGIN_REQUEST")
    {
        handleIncomingAdminConnection();
    }
    else if (receivedMessage == "DRONE_LOGIN_REQUEST")
    {
        handleIncomingDroneConnection();
    }
    else if (receivedMessage == "CONNECTION_LOST")
    {
        std::cout << "Closing ListeningConnection due to : "
            << receivedMessage
            << " received from client" << std::endl;
        reset();
    }
    else
    {
        std::cout << "Received unhandled message: " << receivedMessage << std::endl;
    }
}

void ListeningConnection::handleIncomingAdminConnection(){
    std::cout << "ListeningConnection:: requesting password from incoming Admin connection message\n";
    tcpServer->sendMessage("ADMIN_PASSWORD_REQUEST");
    short attempt{ 0 };
    while (attempt != amountOfAllowedLoginAttempts)
    {
        const auto receivedMessage = tcpServer->receiveMessage();
        const auto messageHeader = getMessageHeader(receivedMessage);
        if (messageHeader == "TCP_RECV_EMPTY")
        {
            continue;
        }
        else if (messageHeader == "SERVER_PASSWORD_IND")
        {
            auto deserializedMsg = deserializeServerPasswordInd(receivedMessage);
            if(passwordMatches(deserializedMsg.password))
            {
                std::cout << "ListeningConnection:: password matched!\n";
                connectedClientId = userIdProvider.generateNextUserId();
                tcpServer->sendMessage(serialize(AdminLoginCfm{ "ADMIN_LOGIN_CFM", serverId, connectedClientId }));
                udpServer->addService(AF_INET, "127.0.0.1", udpPort);
                clientMode = ClientMode::ADMIN;
                connected = true;
                break;
            }
            std::cout << "ListeningConnection:: invalid password!\n";
            tcpServer->sendMessage("ADMIN_PASSWORD_INVALID");
            attempt++;
        }
    }
}

void ListeningConnection::handleIncomingDroneConnection(){
    std::cout << "ListeningConnection:: confirming incoming Drone Unit Connection\n";
    tcpServer->sendMessage("DRONE_LOGIN_CFM");
    clientMode = ClientMode::DRONE;
    connected = true;
}

bool ListeningConnection::foundConnection() const {
    return (connected && tcpServer && udpServer && isConnectionModeDefined());
}

std::unique_ptr<TcpServer> ListeningConnection::releaseTcpPipe() {
    std::cout << "ListeningConnection:: releaseTcpPipe\n";
    return std::move(tcpServer);
}

std::unique_ptr<UdpServer> ListeningConnection::releaseUdpPipe() {
    std::cout << "ListeningConnection:: releaseUdpPipe\n";
    return std::move(udpServer);
}

void ListeningConnection::reset() {
    std::cout << "ListeningConnection:: reset\n";
    close();
    tcpServer.reset(new TcpServer{});
    udpServer.reset(new UdpServer{});
}

void ListeningConnection::close()
{
    if (tcpServer)
    {
        std::cout << "ListeningConnection:: close TCP\n";
        tcpServer->closeConnection();
    }
    if (udpServer)
    {
        std::cout << "ListeningConnection:: close UDP\n";
        udpServer->closeConnection();
    }
    connected = false;
    clientMode = ClientMode::NONE;
    std::cout << "ListeningConnection:: closing pipes: " << std::endl;
}
