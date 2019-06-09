cmake_minimum_required(VERSION 2.8)

project(os-net)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined")

include_directories(${CMAKE_SOURCE_DIR})

add_executable(echo-server server/main.cpp server/server.cpp utils.cpp)

add_executable(echo-client client/main.cpp client/client.cpp utils.cpp)
