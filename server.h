//
// Created by max on 01.05.19.
//

#pragma once

#include <sys/socket.h>
#include <sys/un.h>
#include <array>
#include "helper.h"

class server {
public:
    server() = default;

    explicit server(std::string soc_name);

    server &operator=(const server &) = delete;

    server(const server &) = delete;

    server &operator=(const server &&) = delete;

    server(const server &&) = delete;

    int start();

    ~server();

private:
    int connection_socket{};
    int data_socket{};
    std::array<char, BUFFER_SIZE> buffer{};
    const std::string adding = "Hello, ";
    std::string soc_name;
};


