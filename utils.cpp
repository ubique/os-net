#include "utils.h"

void print_error(std::string const& message) {
    std::cerr << message << ": " << strerror(errno) << '\n';
}

void throw_error(std::string const& message) {
    throw std::runtime_error(message + ": " + strerror(errno));
}
