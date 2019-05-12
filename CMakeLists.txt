cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

project(sntp)

add_executable(sntp-client client.cpp
                           client.h
                           protocol.h)

add_executable(sntp-server server.cpp
                           server.h
                           protocol.h)

install(TARGETS sntp-client
                sntp-server
        RUNTIME DESTINATION bin)
