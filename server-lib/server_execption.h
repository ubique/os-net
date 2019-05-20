//
// Created by Михаил Терентьев on 2019-05-14.
//

#ifndef OS_NET_SERVER_EXECPTION_H
#define OS_NET_SERVER_EXECPTION_H

#include <stdexcept>
#include <cstring>
#include <errno.h>

struct server_exception : std::runtime_error {
    explicit server_exception(std::string const &message) : runtime_error(
            "Server exception: " + message + '\n' + strerror(errno)) {};
};

#endif //OS_NET_SERVER_EXECPTION_H
