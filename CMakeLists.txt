cmake_minimum_required(VERSION 2.8)

project(os-net)
add_executable(server mains/server_main.cpp)
add_executable(client mains/client_main.cpp)

include_directories(${PROJECT_SOURCE_DIR})

set(CXX_STANDARD_REQUIRED 11)

add_library(server-lib STATIC server/server.h server/server.cpp)
set_target_properties(server-lib PROPERTIES LINKER_LANGUAGE CXX)

add_library(client-lib STATIC client/client.h client/client.cpp)
set_target_properties(client-lib PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(server server-lib)
target_link_libraries(client client-lib)
