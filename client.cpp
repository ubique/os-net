#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include "utils.h"

const size_t BUFFER_SIZE = 65508;

void print_help() {
    std::cout << "Usage: ./client IP PORT message" << std::endl;
}

int main(int argc, char **argv) {
    if (argc != 4) {
        std::cerr << "Wrong number of arguments\n";
        print_help();
        return EXIT_FAILURE;
    }
    if (!check_port(argv[2])) {
        std::cout << "Port number is incorrect" << std::endl;
        return EXIT_FAILURE;
    }
    int status;
    struct addrinfo hints{};
    struct addrinfo *result, *chosen;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    if ((status = getaddrinfo(argv[1], argv[2], &hints, &result)) != 0) {
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
        break;
    }
    if (chosen == nullptr) {
        std::cerr << "Socket wasn't created, exiting" << std::endl;
        freeaddrinfo(result);
        return EXIT_FAILURE;
    }
    char buffer[BUFFER_SIZE];
    std::cout << "Sending \"" << argv[3] << "\" to " << argv[1] << ":" << argv[2] << std::endl;
    auto response_len = sendto(descriptor, argv[3], std::strlen(argv[3]), 0, chosen->ai_addr, chosen->ai_addrlen);
    if (response_len == -1) {
        freeaddrinfo(result);
        close_socket(descriptor);
        return EXIT_FAILURE;
    }
    std::cout << "Message has been sent" << std::endl;
    auto request_len = recv(descriptor, &buffer, BUFFER_SIZE - 1, 0);
    if (request_len == -1) {
        freeaddrinfo(result);
        close_socket(descriptor);
        return EXIT_FAILURE;
    }
    buffer[request_len] = 0;
    std::cout << "Response received: " << buffer << std::endl;
    freeaddrinfo(result);
    close_socket(descriptor);

}
