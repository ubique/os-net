#include "utils.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <numeric>
#include <netdb.h>


void print_error(std::string const& message) {
    std::cerr << message << std::strerror(errno) << std::endl;
}

bool check_port(const char* port) {
    uint32_t value = 0;
    for (const char *ptr = port; *ptr != 0; ptr++) {
        if (!isdigit(*ptr)) {
            return false;
        }
        value = value * 10 + (*ptr - '0');
        if (value > std::numeric_limits<uint16_t>::max()) {
            return false;
        }
    }
    return true;
}

void send_all(const char* pointer, size_t len, int descriptor) {
    ssize_t result;
    while (len && (result = send(descriptor, pointer, len, 0))) {
        if (result < 0 && errno == EINTR) {
            continue;
        }
        if (result < 0) {
            print_error("send failed");
        }
        len -= result;
        pointer += result;
    }
}

std::string recv_all(int descriptor, char* buffer, size_t buffer_size) {
    std::string received;
    ssize_t received_len;
    while ((received_len = recv(descriptor, buffer, buffer_size - 1, 0)) > 0) {
        buffer[received_len] = '\0';
        received += buffer;
        if (buffer[received_len - 1] == '\0') {
            break;
        }
    }
    if (received_len < 0) {
        print_error("recv failed: ");
        return "";
    }
    return received;
}
