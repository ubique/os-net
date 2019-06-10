cmake_minimum_required(VERSION 3.00)
project(os_net)

set(CMAKE_CXX_STANDARD 14)


add_executable(os-net-client
        Client.cpp
        Client.h
        Utils.h)

add_executable(os-net-server
        Server.cpp
        Server.h
        Utils.h
        )

install(TARGETS os-net-client
        os-net-server
        RUNTIME DESTINATION bin
        )