#include "socket_wrapper.hpp"

socket_wrapper::socket_wrapper() : descriptor(socket(AF_INET, SOCK_DGRAM, 0)) {}

socket_wrapper::socket_wrapper(int descriptor) : descriptor(descriptor) {}

socket_wrapper::~socket_wrapper() {
    close();
}

int socket_wrapper::get_descriptor() const {
    return descriptor;
}

bool socket_wrapper::check_valid() const {
    return descriptor != -1;
}

void socket_wrapper::close() {
    if (::close(descriptor)) {
        logger().fail("Could not close the socket properly", errno);
    }
}