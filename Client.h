#pragma once

#include <bits/exception.h>
#include <stdexcept>


class Client {
public:
    Client(char *address, uint16_t port);

    ~Client();

    std::string makeRequest(std::string message);

private:
    int mySocket;
    static constexpr size_t BUFFER_SIZE = 1024;

    void openSocket();

    void makeConnection(char *address, uint16_t port);
};


