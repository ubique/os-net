//
// Created by dumpling on 20.05.19.
//

#include <iostream>
#include <cstring>
#include <limits>
#include <sys/socket.h>

void print_err(const std::string &message) {
    std::cerr << "\033[31m" << message;
    if (errno) {
        std::cerr << ": " << std::strerror(errno);
    }
    std::cerr << "\033[0m" << std::endl;
}

uint16_t get_port(const std::string &number) {

    if (number[0] == '-') {
        throw std::invalid_argument("Port should be not negative");
    }

    size_t st = 0;
    auto real_number = std::stoi(number, &st);

    if (st != number.size() || real_number > std::numeric_limits<uint16_t>::max()) {
        throw std::invalid_argument("Port should be a positive integer number less than 2^16");
    }

    return real_number;
}

void send_all(int sfd, const char *buf, int size) {
    int total = 0;
    while (total < size) {
        int was_send = send(sfd, buf + total, size - total, 0);
        if (was_send == -1) {
            throw std::runtime_error("Send failed");
        }

        total += was_send;
    }
}