project(Server LANGUAGES CXX)
cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(PROJECT_SOURCES
    main.cpp
    ServerApp.cpp
    ServerApp.hpp
    UserIdProvider.cpp
    UserIdProvider.hpp
    ListeningConnection.cpp
    ListeningConnection.hpp
    InstanceCreator.cpp
    InstanceCreator.hpp
    ConnectionManager.cpp
    ConnectionManager.hpp
    AdminInstance.cpp
    AdminInstance.hpp
    DroneInstance.cpp
    DroneInstance.hpp
    AdminConnectionProxy.cpp
    AdminConnectionProxy.hpp
    DroneConnectionProxy.cpp
    DroneConnectionProxy.hpp
    DroneConnectionStub.cpp
    DroneConnectionStub.hpp
)
add_subdirectory(ServerNetwork)
add_executable (Server main.cpp ${PROJECT_SOURCES})
target_include_directories(Server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Server PRIVATE ServerNetwork)
target_link_libraries(Server PRIVATE Utilities)
set_property(TARGET Server PROPERTY CXX_STANDARD 23)
