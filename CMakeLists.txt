cmake_minimum_required(VERSION 3.10)
project(prog)
set(CMAKE_CXX_STANDARD 17)
file(GLOB SOURCES "./*.cpp" "./*.h")

add_executable(server rserver.cpp server.cpp server.h)
add_executable(client rclient.cpp client.cpp client.h)
