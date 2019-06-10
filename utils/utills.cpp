//
// Created by Anton Shelepov on 2019-06-10.
//

#include "utils.h"
#include <string>
#include <vector>
#include <sys/socket.h>
#include <zconf.h>

std::string utils::read(int desc, size_t expected) {
    std::vector<char> buffer;
    size_t tries_to_read = TRIES_NUMBER;
    std::string res;
    do {
        buffer.resize(BUFFER_SIZE);
        ssize_t cnt = ::read(desc, buffer.data(), static_cast<size_t>(buffer.size()));
        if (cnt == 0) {
            break;
        }
        if (cnt != -1) {
            buffer.resize((size_t) cnt);
            res.append(buffer.data());
        }
    } while (res.size() < expected && tries_to_read--);

    return res;
}

size_t utils::send(int desc, std::string const& message) {
    ssize_t was_sent = 0;
    size_t tries_to_send = TRIES_NUMBER;
    while (was_sent < message.size() && tries_to_send--) {
        ssize_t cnt = ::write(desc, message.data() + was_sent, static_cast<size_t>(message.size() - was_sent));
        if (cnt != -1) {
            was_sent += cnt;
        }
    }

    return (size_t) was_sent;
}