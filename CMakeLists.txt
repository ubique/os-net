cmake_minimum_required(VERSION 3.0)
project(CLIENT_SERVER)

set(CMAKE_CXX_STANDARD 14)

include_directories(./headers ./src)
add_library(Server STATIC ./src/POP3Server.cpp ./headers/POP3Server.h headers/Message.h headers/User.h src/User.cpp src/Message.cpp src/Session.cpp src/DBase.cpp src/Utils.cpp)

add_executable(client client.cpp headers/Session.h headers/DBase.h headers/Utils.h src/Utils.cpp)
add_executable(server server.cpp)
target_link_libraries(server Server)