#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include "socket_handler.h"

const size_t BUFFER_SIZE = 65507;

void print_help() {
    std::cout << "Usage: ./client IP PORT message" << std::endl;
}

int main(int argc, char **argv) {
    if (argc != 4) {
        std::cerr << "Wrong number of arguments\n";
        print_help();
        return EXIT_FAILURE;
    }
    uint16_t port;
    try {
         port = static_cast<uint16_t>(std::stoul(argv[2]));
    } catch (...) {
        std::cerr << "Port number is incorrect" << std::endl;
        return EXIT_FAILURE;
    }
    try {
        socket_handler udp_socket;
        struct sockaddr_in server_address{};
        try {
            struct in_addr addr;
            if (inet_aton(argv[1], &addr) == 0) {
                std::cerr << "Incorrect ip address " << std::endl;
                return EXIT_FAILURE;
            }
            server_address.sin_addr = addr;
        } catch (...){
            return EXIT_FAILURE;
        }
        server_address.sin_port = htons(port);
        server_address.sin_family = AF_INET;
        char buffer[BUFFER_SIZE];
        socklen_t len;
        std::cout << "Sending \"" << argv[3] << "\" to " << argv[1] << ":" << port << std::endl;
        auto response_len = sendto(udp_socket.get_descriptor(), argv[3], std::strlen(argv[3]), 0, (struct sockaddr *)&server_address, sizeof(server_address));
        if (response_len == -1) {
            return EXIT_FAILURE;
        }
        std::cout << "Message has been sent" << std::endl;
        auto request_len = recvfrom(udp_socket.get_descriptor(), &buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_address, &len);
        if (request_len == -1) {
            return EXIT_FAILURE;
        }
        buffer[request_len] = 0;
        std::cout << "Response received: " << buffer << std::endl;

    } catch(...) {
        return EXIT_FAILURE;
    }
}
