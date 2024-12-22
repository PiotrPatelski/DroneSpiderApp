add_library(ServerNetwork "TcpServer.cpp" "TcpServer.hpp" "UdpServer.cpp" "UdpServer.hpp" )

target_include_directories(ServerNetwork PUBLIC .)
