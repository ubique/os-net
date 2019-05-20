#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include "utils.h"
#include <cstring>

const size_t BUFFER_SIZE = 65508;

void print_help() {
    std::cout << "Usage: ./server PORT" << std::endl;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Incorrect number of arguments\n";
        print_help();
        return EXIT_FAILURE;
    }
    if (!check_port(argv[1])) {
        std::cout << "Port number is incorrect" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Starting server on port " << argv[1] << std::endl;
    int status;
    struct addrinfo hints{};
    struct addrinfo *result, *chosen = nullptr;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    if ((status = getaddrinfo(nullptr, argv[1], &hints, &result)) != 0) {
        std::cerr << "getaddrinfo failed: " << gai_strerror(status);
        freeaddrinfo(result);
        return EXIT_FAILURE;
    }
    int descriptor = -1;
    for (chosen = result; chosen != nullptr; chosen = chosen->ai_next) {
        descriptor = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (descriptor == -1) {
            print_error("Can't create socket, trying next: ");
            continue;
        }
        if (::bind(descriptor, result->ai_addr, result->ai_addrlen) == -1) {
            print_error("Can't bind socket, trying next: ");
            close_socket(descriptor);
            continue;
        }
        break;
    }
    if (chosen == nullptr) {
        std::cerr << "Socket wasn't created/binded, exiting" << std::endl;
        freeaddrinfo(result);
        return EXIT_FAILURE;
    }
    char buffer[BUFFER_SIZE];
    while (true) {
        struct sockaddr_in client_address{};
        socklen_t len =  sizeof (client_address);
        auto request_len = recvfrom(descriptor, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&client_address, &len);
        if (request_len < 0) {
            print_error("recvfrom failed: ");
            continue;
        }
        buffer[request_len] = 0;
        std::cout << "received: " << buffer << std::endl;
        auto response_len = sendto(descriptor, buffer, request_len, 0, (struct sockaddr *)&client_address, len);
        if (response_len == -1) {
            print_error("sendto failed: ");
            continue;
        }
    }
    close_socket(descriptor);
    freeaddrinfo(result);
}
