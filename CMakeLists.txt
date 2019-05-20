cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

project(tftp)

add_executable(server "server.cpp")
add_executable(client "client.cpp")

add_library(utils STATIC "utils.cpp" "utils.h")

target_link_libraries(server utils)
target_link_libraries(client utils)
