cmake_minimum_required(VERSION 3.0)
project(os_net)

set(CMAKE_CXX_STANDARD 14)

add_executable(os_net_server server.cpp Server.cpp Server.h)
add_executable(os_net_client client.cpp Client.cpp Client.h)

cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)


install(TARGETS os_net_server
        os_net_client
        RUNTIME DESTINATION bin)