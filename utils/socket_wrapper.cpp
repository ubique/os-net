#include "socket_wrapper.hpp"

socket_wrapper::socket_wrapper() : descriptor(socket(AF_INET, SOCK_STREAM, 0)) {}

socket_wrapper::socket_wrapper(int descriptor) : descriptor(descriptor) {}

socket_wrapper::~socket_wrapper() {
    close();
}

void socket_wrapper::renew() {
    close();
    descriptor = socket(AF_INET, SOCK_STREAM, 0);
}

int socket_wrapper::get_descriptor() const {
    return descriptor;
}

bool socket_wrapper::check_valid() const {
    return descriptor != -1;
}

int socket_wrapper::send_message(char const *buf, size_t buffer_size, unsigned int repeat, bool log_success) {
    if (!check_valid()) {
        logger().fail("Socket descriptor is invalid, can not sent message");
        return -1;
    }
    int attempt = 0;
    size_t offset = 0;
    ssize_t sent;
    while (attempt++ < repeat) {
        sent = send(descriptor, reinterpret_cast<void const *>(buf + offset), buffer_size - offset, 0);
        if (sent == -1) {
            continue;
        }
        attempt = 0;
        offset += sent;
        if (offset == buffer_size) {
            if (log_success) {
                logger().success("Sent '" + std::string(buf, offset - 1) + "' of size " + std::to_string(buffer_size));
            }
            return 0;
        }
    }
    logger().fail("Could not send message", errno);
    return -1;
}

int socket_wrapper::receive_message(char *buf, size_t &buffer_size,
                                    unsigned int real_size, unsigned int repeat, bool log_success) {
    if (!check_valid()) {
        logger().fail("Socket descriptor is invalid, can not receive message");
        return -1;
    }
    int attempt = 0;
    size_t offset = 0;
    ssize_t received;
    while (attempt++ < repeat) {
        received = recv(descriptor, reinterpret_cast<void *>(buf + offset), real_size - offset, 0);
        if (received == -1) {
            continue;
        }
        attempt = 0;
        offset += received;
        if (buf[offset - 1] == '\n') {
            buf[offset - 1] = '\0';
            buffer_size = offset - 1;
            if (log_success) {
                logger().success("Received '" + std::string(buf) + "' of size " + std::to_string(offset));
            }
            return 0;
        }
    }
    logger().fail("Could not read message", errno);
    return -1;
}

void socket_wrapper::close() {
    if (check_valid() && ::close(descriptor) < 0) {
        logger().fail("Could not close the socket properly", errno);
    }
    descriptor = -1;
}
