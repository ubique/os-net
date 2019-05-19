cmake_minimum_required(VERSION 3.0)
project(find)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -pedantic -D_GLIBCXX_DEBUG")

include_directories(${PROJECT_SOURCE_DIR})
set(UTILS utils.cpp utils.hpp)
add_executable(TCPserver TCPserver.cpp ${UTILS})
add_executable(TCPclient TCPclient.cpp ${UTILS})