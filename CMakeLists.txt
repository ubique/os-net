cmake_minimum_required(VERSION 3.10)
project(os_net)

set(CMAKE_CXX_STANDARD 14)

add_executable(os_net_server main_server.cpp server.cpp server.h utils.cpp utils.h)
add_executable(os_net_client main_client.cpp client.cpp client.h utils.cpp utils.h)
