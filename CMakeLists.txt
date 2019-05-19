cmake_minimum_required(VERSION 2.8)
project(os-net)

set(CMAKE_CXX_STANDARD 14)

add_executable(server server.h server.cpp run_server.cpp)
add_executable(client client.h client.cpp run_client.cpp)