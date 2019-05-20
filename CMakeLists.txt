cmake_minimum_required(VERSION 2.8)

project(net)

set(CMAKE_CXX_STANDARD 14)

add_executable(client "client.cpp")
add_executable(server "server.cpp")
