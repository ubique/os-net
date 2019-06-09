cmake_minimum_required(VERSION 3.12)
project(Task5)

set(CMAKE_CXX_STANDARD 14)

add_executable(Task5_client client.cpp utils.cpp)
add_executable(Task5_server server.cpp utils.cpp)

#cmake_minimum_required(VERSION 3.10)
#project(os-net)
#
#set(CMAKE_CXX_STANDARD 14)
#
#add_executable(os-net-server server.cpp utils.cpp utils.h)
#add_executable(os-net-client client.cpp utils.cpp utils.h)