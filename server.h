//
// Created by roman on 20.05.19.
//

#ifndef OS_NET_SERVER_H
#define OS_NET_SERVER_H


#include <array>
#include <netinet/in.h>
#include <string>
#include "my_fd.h"
#include "my_error.h"

class server {
public:
    server();

    void start(const char *, const in_port_t);

private:
    void response(int client_fd, ssize_t count);
    ssize_t receive_request(int client_fd);
private:
    static constexpr size_t BUFFER_SIZE = 4096;
    my_fd socket_fd;
    char buffer[BUFFER_SIZE]; //for null terminated string
};


#endif //OS_NET_SERVER_H
