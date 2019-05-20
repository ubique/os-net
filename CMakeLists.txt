cmake_minimum_required(VERSION 3.9)

project(os-net)

add_executable(server mainServer.cpp Server.cpp)
add_executable(client mainClient.cpp Client.cpp)