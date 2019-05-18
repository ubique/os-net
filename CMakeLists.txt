CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
project(os_net)

set(CMAKE_CXX_STANDARD 14)

add_executable(echo_server socket_descriptor/socket_descriptor.h socket_descriptor/socket_descriptor.cpp server/server.h server/server.cpp)
add_executable(echo_client socket_descriptor/socket_descriptor.h socket_descriptor/socket_descriptor.cpp client/client.h client/client.cpp)