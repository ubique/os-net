cmake_minimum_required(VERSION 3.13)
project(os-net)

set(CMAKE_CXX_STANDARD 14)

#set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined")

add_executable(server hello_server.cpp hello_server.h server_main.cpp error.cpp error.h)
add_executable(client client.cpp client.h client_main.cpp error.cpp error.h)