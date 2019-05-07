//
// Created by max on 01.05.19.
//

#pragma once

#include <sys/socket.h>
#include <sys/un.h>
#include <string>
#include "helper.h"
#include "Socket.h"
#include <array>


class client {
public:
    explicit client(std::string host);

    client() = default;

    int send(const std::string &msg);

    ~client();

    client &operator=(const client &) = delete;

    client(const client &) = delete;

    client &operator=(const client &&) = delete;

    client(const client &&) = delete;

private:
    Socket socket{};
    std::string host;
};
