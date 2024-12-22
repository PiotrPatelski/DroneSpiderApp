#pragma once
#include "ConnectionManager.hpp"

class ServerApp {
public:
    ServerApp();
    ~ServerApp();

    void start();

private:
    ConnectionManager connectionManager;
};