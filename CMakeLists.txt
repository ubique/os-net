cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

project(tftp)

add_executable(server "server.cpp")
add_executable(client "client.cpp")

add_library(socket_handler STATIC "socket_handler.cpp" "socket_handler.h")

target_link_libraries(server socket_handler)
target_link_libraries(client socket_handler)
