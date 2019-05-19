cmake_minimum_required(VERSION 3.10)
project(task5)

set(CMAKE_CXX_STANDARD 14)

add_executable(server server_run.cpp Server.cpp Server.h)
add_executable(client client_run.cpp Client.cpp Client.h)