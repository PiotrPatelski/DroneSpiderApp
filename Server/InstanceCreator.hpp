#pragma once
#include "InstanceCreator.hpp"
#include "AdminInstance.hpp"
#include "DroneInstance.hpp"
#include "ListeningConnection.hpp"
#include <memory>
#include <vector>

class InstanceCreator {
public:
    InstanceCreator();
    ~InstanceCreator();

    std::unique_ptr<ListeningConnection> createListeningConnection();
    std::unique_ptr<AdminInstance> createAdminInstance(ListeningConnection& acceptedConnection);
    std::unique_ptr<DroneInstance> createDroneInstance(ListeningConnection& acceptedConnection);
    //TODO REMOVE STUB CODE
    std::unique_ptr<DroneInstance> createDroneStub(ListeningConnection& acceptedConnection);
    //
};