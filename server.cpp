#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include "socket_handler.h"

const size_t BUFFER_SIZE = 65507;

void print_help() {
    std::cout << "Usage: ./server PORT" << std::endl;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Wrong number of arguments\n";
        print_help();
        return EXIT_FAILURE;
    }
    uint16_t port;
    try {
         port = static_cast<uint16_t>(std::stoul(argv[1]));
    } catch (...) {
        std::cout << "Port number is incorrect" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Starting server on port " << port << std::endl;
    try {
        socket_handler udp_socket;
        try {
            udp_socket.bind(INADDR_ANY, port);
        } catch (...){
            return EXIT_FAILURE;
        }
        char buffer[BUFFER_SIZE];
        struct sockaddr_in client_address{};
        socklen_t len;
        while (true) {
            auto request_len = recvfrom(udp_socket.get_descriptor(), &buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_address, &len);
            if (request_len == -1) {
                continue;
            }
            buffer[request_len] = 0;
            std::cout << "received: " << buffer << std::endl;
            auto response_len = sendto(udp_socket.get_descriptor(), &buffer, request_len, 0, (struct sockaddr *)&client_address, len);
            if (response_len == -1) {
                continue;
            }
        }
    } catch(...) {
        return EXIT_FAILURE;
    }
}
