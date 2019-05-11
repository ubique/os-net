cmake_minimum_required(VERSION 3.0)
project(CLIENT_SERVER)

set(CMAKE_CXX_STANDARD 14)

include_directories(./headers ./src)
add_library(Server STATIC ./src/POP3Server.cpp ./headers/POP3Server.h headers/Message.h headers/User.h src/User.cpp src/Message.cpp)

add_executable(client client.cpp)
add_executable(server server.cpp)
target_link_libraries(server Server)