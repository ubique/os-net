cmake_minimum_required(VERSION 3.10)

project(net)

add_library(utilslib
        utils/logger.cpp utils/logger.hpp
        utils/socket_wrapper.cpp utils/socket_wrapper.hpp)

add_executable(client client.cpp client.hpp)
add_executable(server server.cpp server.hpp)
target_link_libraries(client utilslib)
target_link_libraries(server utilslib)