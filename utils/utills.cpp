//
// Created by Anton Shelepov on 2019-06-10.
//

#include "utils.h"
#include <string>
#include <vector>
#include <sys/socket.h>

std::string utils::read(int desc) {
    std::vector<char> buffer(BUFFER_SIZE);
    ssize_t was_read = 0;
    size_t tries_to_read = TRIES_NUMBER;
    while (tries_to_read) {
        ssize_t cnt = ::recv(desc, buffer.data(), static_cast<size_t>(buffer.size()), 0);
        if (cnt == -1) {
            tries_to_read--;
            continue;
        }
        was_read += cnt;
        break;
    }

    buffer.resize((size_t) was_read);
    return std::string(buffer.data());
}

size_t utils::send(int desc, std::string const& message) {
    ssize_t was_sent = 0;
    size_t tries_to_send = TRIES_NUMBER;
    while (was_sent < message.size() && tries_to_send) {
        ssize_t cnt = ::send(desc, message.data() + was_sent, static_cast<size_t>(message.size() - was_sent), 0);
        if (cnt == -1) {
            tries_to_send--;
            continue;
        }
        was_sent += cnt;
    }

    return (size_t) was_sent;
}