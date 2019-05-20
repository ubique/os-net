//
// Created by daniil on 19.05.19.
//

#ifndef OS_NET_CLIENT_H
#define OS_NET_CLIENT_H

#include <string>
#include <stdexcept>
#include <netinet/in.h>
#include <unistd.h>

struct client_exception : std::runtime_error {
    explicit client_exception(const std::string& message);
};

class Client {
    int socket;
    sockaddr_in address{};
    const int SIZE = 2048;

public:
    Client(char* clientAddress, uint16_t port);
    std::string request(const std::string& text);

};


#endif //OS_NET_CLIENT_H
