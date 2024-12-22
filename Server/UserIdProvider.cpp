#include "UserIdProvider.hpp"
#include <stdexcept>
#include <iostream>

static constexpr unsigned short lowestPossibleUserId{ 1 };

UserIdProvider::UserIdProvider() : nextUserId(lowestPossibleUserId) 
{
    std::cout << "UserIdProvider:: init\n";
}

unsigned short UserIdProvider::generateNextUserId()
{
    std::cout << "UserIdProvider:: generate Id for incoming client\n";
    if (idsInUse.size() == std::numeric_limits<unsigned short>::max()) {
        throw std::runtime_error("All possible User IDs are in use.");
    }
    while (std::find(idsInUse.begin(), idsInUse.end(), nextUserId) != idsInUse.end()) {
        incrementUserId();
    }
    std::cout << "UserIdProvider:: generated Id successfully\n";
    idsInUse.push_back(nextUserId);
    return nextUserId;
}

void UserIdProvider::freeUserId(unsigned short userId)
{
    std::cout << "UserIdProvider:: free Id for closing client\n";
    auto foundUserId = std::find(idsInUse.begin(), idsInUse.end(), userId);
    if (foundUserId != idsInUse.end()) {
        std::cout << "UserIdProvider:: freed Id successfully\n";
        idsInUse.erase(foundUserId);
    }
}

void UserIdProvider::incrementUserId() {
    if (nextUserId == std::numeric_limits<unsigned short>::max()) {
        nextUserId = lowestPossibleUserId;
    }
    else {
        ++nextUserId;
    }
}