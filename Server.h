//
// Created by daniil on 18.05.19.
//

#ifndef OS_NET_SERVER_H
#define OS_NET_SERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <string>
#include <unistd.h>

struct server_exception : std::runtime_error {
    explicit server_exception(const std::string& message);
};

class Server {
    sockaddr_in address{};
    int socket{};

public:
    Server() = default;
    explicit Server(char* address, uint16_t port);

    void run();

};


#endif //OS_NET_SERVER_H
