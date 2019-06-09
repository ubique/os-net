#include <sys/socket.h>
#include <stdexcept>
#include <unistd.h>
#include "utils.h"

const size_t BUFFER_SIZE = 1 << 16;

std::pair<std::string, ssize_t> socket_read(int sock, bool throw_flag) {
    char buf[BUFFER_SIZE];
    ssize_t reading = 0;

    while (reading != BUFFER_SIZE) {
        ssize_t cur_size = read(sock, buf + reading, BUFFER_SIZE - reading);
        if (cur_size == -1) {
            if (throw_flag) {
                throw std::runtime_error("Reading error");
            } else {
                return {"", -1};
            }
        }
        reading += cur_size;
        if (buf[reading - 1] == '\0')
            break;
    }

    return {buf, reading};
}

void socket_send(int sock, std::string &str, bool throw_flag) {
    const char *message = str.c_str();
    size_t str_len = sizeof(message);

    ssize_t sending = 0;

    do {
        ssize_t cur_size = send(sock, message + sending, str_len - sending, 0);
        if (cur_size == -1) {
            if (throw_flag) {
                throw std::runtime_error("Sending error");
            } else {
                perror("Sending error");
            }
        }
        sending += cur_size;
    } while (sending != str_len);
}