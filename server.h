//
// Created by max on 01.05.19.
//

#pragma once

#include <sys/socket.h>
#include <sys/un.h>
#include <array>
#include "helper.h"
#include "Socket.h"

class server {
public:
    server() = default;

    explicit server(const std::string &soc_name);

    server &operator=(const server &) = delete;

    server(const server &) = delete;

    server &operator=(const server &&) = delete;

    server(const server &&) = delete;

    int start();

    ~server();

private:
    Socket socket{};
    const std::string adding = "Hello, ";
};


