#pragma once

#include <bits/exception.h>
#include <stdexcept>


class Server {
public:
    Server(char *address, uint16_t port);

    ~Server();

    void start();

private:
    int mySocket;

    static constexpr size_t BUFFER_SIZE = 1024;

    void openSocket();

    void setUp(char *address, uint16_t port);
};


