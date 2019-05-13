cmake_minimum_required(VERSION 3.13)
project(os_net)

set(CMAKE_CXX_STANDARD 14)

add_executable(server server.cpp POP_server.cpp POP_server.h fd_wrapper.h Mail_DB.cpp Mail_DB.h)
add_executable(client client.cpp)
