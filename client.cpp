#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include "utils.h"

const size_t BUFFER_SIZE = 65536;

void print_help() {
    std::cout << "Usage: ./client IP PORT" << std::endl;
}

int main(int argc, char **argv) {
    if (argc != 3) {
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
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((status = getaddrinfo(argv[1], argv[2], &hints, &result)) != 0) {
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
        break;
    }
    if (chosen == nullptr) {
        std::cerr << "Socket wasn't created, exiting" << std::endl;
        freeaddrinfo(result);
        return EXIT_FAILURE;
    }
    descriptor_wrapper server_descriptor = connect(descriptor, chosen->ai_addr, chosen->ai_addrlen);
    if (server_descriptor == -1) {
        print_error("connect failed: ");
        freeaddrinfo(result);
        return EXIT_FAILURE;
    }
    std::cout << "Connected to server\nType exit to exit\nType stop to stop server" << std::endl;
    char buffer[BUFFER_SIZE];
    while (true) {
        std::string line;
        std::cout << "Enter message to send: ";
        getline(std::cin, line);
        if (std::cin.eof()) {
            break;
        }
        if (line == "") {
            continue;
        }
        send_all(line.c_str(), line.size() + 1, descriptor);
        if (line == "exit" || line == "stop") {
            break;
        }
        auto response = recv_all(descriptor, buffer, BUFFER_SIZE);
        if (response == "") {
            break;
        }
        std::cout << "response: " << response << std::endl;
    }
    freeaddrinfo(result);
}
