cmake_minimum_required(VERSION 2.8.11)
project(os-net)

set(CMAKE_CXX_STANDARD 14)

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -D_GLIBCXX_DEBUG ")
endif ()


add_executable(net-server server.cpp server.h main-server.cpp helper.h helper.cpp Socket.cpp )

add_executable(net-client helper.h helper.cpp client.cpp client.h main-client.cpp Socket.cpp )