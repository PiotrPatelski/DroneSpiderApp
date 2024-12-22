add_library(ClientNetwork "TcpClient.cpp" "TcpClient.hpp" "UdpClient.cpp" "UdpClient.hpp" )

target_include_directories(ClientNetwork PUBLIC .)
