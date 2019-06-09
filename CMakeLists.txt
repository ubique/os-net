cmake_minimum_required(VERSION 3.10)
project(find)

set(CMAKE_CXX_STANDARD 17)

add_executable(echo datagram_packet.h datagram_socket.h datagram_socket.cpp echo_server.h echo_server.cpp debug_echo_client.h debug_echo_client.cpp main.cpp)

find_package(Boost REQUIRED COMPONENTS program_options)

include_directories(${Boost_INCLUDE_DIR})

target_link_libraries(echo ${Boost_LIBRARIES}) 
