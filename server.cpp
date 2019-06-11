#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include "utils.h"
#include <cstring>

const size_t BUFFER_SIZE = 65536;

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
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((status = getaddrinfo(nullptr, argv[1], &hints, &result)) != 0) {
        std::cerr << "getaddrinfo failed: " << gai_strerror(status);
        freeaddrinfo(result);
        return EXIT_FAILURE;
    }
    descriptor_wrapper descriptor = -1;
    for (chosen = result; chosen != nullptr; chosen = chosen->ai_next) {
        descriptor = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (descriptor == -1) {
            print_error("Can't create socket, trying next: ");
            continue;
        }
        if (::bind(descriptor, result->ai_addr, result->ai_addrlen) == -1) {
            print_error("Can't bind socket, trying next: ");
            continue;
        }
        break;
    }
    if (chosen == nullptr) {
        std::cerr << "Socket wasn't created/binded, exiting" << std::endl;
        freeaddrinfo(result);
        return EXIT_FAILURE;
    }
    if (listen(descriptor, SOMAXCONN) == -1) {
        print_error("listen failed: ");
        freeaddrinfo(result);
        return EXIT_FAILURE;
    }
    char buffer[BUFFER_SIZE];
    bool alive = true;
    while (alive) {
        struct sockaddr client_addr;
        socklen_t len = sizeof(client_addr);
        descriptor_wrapper client_descriptor = accept(descriptor, (sockaddr *)&client_addr, &len);
        std::cout << "Connected to the client with descriptor " << client_descriptor << std::endl;
        while (true) {
            auto received = recv_all(client_descriptor, buffer, BUFFER_SIZE);
            if (received == "") {
                break;
            }
            std::cout << "received from " << client_descriptor << ": " << received << std::endl;
            if (received == "exit") {
                break;
            }
            if (received == "stop") {
                alive = false;
                break;
            }
            send_all(received.c_str(), received.size() + 1, client_descriptor);
        }
    }
    freeaddrinfo(result);
}
