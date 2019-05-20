#include "utils.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <numeric>

void print_error(std::string const& message) {
    std::cerr << message << std::strerror(errno) << std::endl;
}

void close_socket(int descriptor) {
    if (close(descriptor) == -1) {
        print_error("Can't close socket");
    }
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
