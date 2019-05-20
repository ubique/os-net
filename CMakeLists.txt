cmake_minimum_required(VERSION 3.12)
project(os_net)

set(CMAKE_CXX_STANDARD 14)

add_executable( server starter/server_starter.cpp)
add_executable( client starter/client_starter.cpp)

add_library(server-lib STATIC
        server-lib/server.h
        server-lib/server.cpp
        server-lib/server_execption.h)
add_library(client-lib STATIC
        client-lib/client.h
        client-lib/client.cpp
        client-lib/client_exception.h)

set_target_properties(server-lib PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(client-lib PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(server server-lib)
target_link_libraries(client client-lib)

