//
// Created by roman on 20.05.19.
//

#ifndef OS_NET_SERVER_H
#define OS_NET_SERVER_H


#include <array>
#include <netinet/in.h>
#include <string>
#include "my_fd.h"

class server
{
public:
    server();

    void start(const char*, const in_port_t);
private:
    static constexpr size_t BUFFER_SIZE = 4096;
    my_fd socket_fd;
    char buffer[BUFFER_SIZE]; //for null terminated string
};


#endif //OS_NET_SERVER_H
