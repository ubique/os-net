cmake_minimum_required(VERSION 3.12)

project(tftp)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address,undefined")

add_library(tftp
        tftp_peer.h tftp_peer.c
        tftp_ec.h
        tftp_packet.h tftp_packet.c
        utils.h tftp.c tftp.h)

add_executable(run-server run-server.c)
add_executable(run-client run-client.c)

target_link_libraries(run-server tftp)
target_link_libraries(run-client tftp)

