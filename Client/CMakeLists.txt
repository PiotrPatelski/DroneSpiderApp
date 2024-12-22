cmake_minimum_required(VERSION 3.16)
project(Client LANGUAGES CXX)

include(qt.cmake)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR}
    COMPONENTS
        Core
        Gui
        Widgets
)
qt_standard_project_setup()

set(PROJECT_SOURCES
    main.cpp
    TcpUdpQtAppUi.hpp
    TcpUdpQtApp.hpp
    TcpUdpQtApp.cpp
    ConnectionWindowUi.hpp
    ConnectionWindowUi.cpp
    ConnectionWindow.hpp
    ConnectionWindow.cpp
    Logger.hpp
    Logger.cpp
    UserPanelUi.hpp
    UserPanelUi.cpp
    UserPanel.hpp
    UserPanel.cpp
    UserConnectionProxy.hpp
    UserConnectionProxy.cpp
    LoginPanelUi.hpp
    LoginPanelUi.cpp
    LoginPanel.hpp
    LoginPanel.cpp
    MoveRequestInputWindow.hpp
    MoveRequestInputWindow.cpp
    MoveRequestInputWindowUi.hpp
    MoveRequestInputWindowUi.cpp
    RotateRequestInputWindow.hpp
    RotateRequestInputWindow.cpp
    RotateRequestInputWindowUi.hpp
    RotateRequestInputWindowUi.cpp
)

qt_add_executable(Client ${PROJECT_SOURCES})
add_subdirectory(ClientNetwork)
target_include_directories(Client PUBLIC ClientNetwork ./)
set_property(TARGET Client PROPERTY CXX_STANDARD 23)
set_target_properties(Client
    PROPERTIES
        WIN32_EXECUTABLE TRUE
)

target_link_libraries(Client
    PUBLIC
        Qt::Core
        Qt::Gui
        Qt::Widgets
        ClientNetwork
        Utilities
)
