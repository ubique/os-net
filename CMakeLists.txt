cmake_minimum_required(VERSION 3.12.1)
project(os-net)

set(CMAKE_CXX_STANDARD 14)

#set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined")

add_executable(server server.cpp server.h server_main.cpp my_error.cpp my_error.h my_fd.h my_fd.cpp)
add_executable(client client.cpp client.h client_main.cpp my_error.cpp my_error.h my_fd.h my_fd.cpp)