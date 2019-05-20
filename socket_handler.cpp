#include "socket_handler.h"

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <stdexcept>

void print_error(std::string const& message) {
    std::cerr << message << std::strerror(errno) << std::endl;
}

socket_handler::socket_handler() {
    descriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if (descriptor == -1) {
        print_error("Can't create socket: ");
        throw std::runtime_error("Can't create socket");
    }
}

socket_handler::~socket_handler() {
    if (close(descriptor) == -1) {
        print_error("Can't close socket: ");
    }
}

void socket_handler::bind(uint32_t addr, uint16_t port) {
    struct sockaddr_in server_address{};
    server_address.sin_addr.s_addr = addr;
    server_address.sin_port = htons(port);
    server_address.sin_family = AF_INET;
    if (::bind(descriptor, (const struct sockaddr *)(&server_address), sizeof(server_address)) == -1) {
        print_error("Can't bind socket: ");
        throw std::runtime_error("Can't bind socket");
    }
}

int socket_handler::get_descriptor() {
    return descriptor;
}
