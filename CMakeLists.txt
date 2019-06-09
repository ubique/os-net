cmake_minimum_required(VERSION 3.11)
project(net)

set(CMAKE_CXX_STANDARD 14)

add_executable(server server.cpp)
add_executable(client client.cpp)
