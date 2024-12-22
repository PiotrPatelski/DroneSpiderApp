#include "ServerApp.hpp"
#include "UnitContext.hpp"
#include "InstanceCreator.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>

ServerApp::ServerApp()
    : connectionManager(std::make_unique<InstanceCreator>())
{
    std::cout << "ServerApp:: init\n";
}

ServerApp::~ServerApp() {
    std::cout << "ServerApp:: closing\n";
}

void ServerApp::start() {
    std::cout << "ServerApp:: start\n";
    connectionManager.start();
}