#ifndef HW5_NET_SERVER_H
#define HW5_NET_SERVER_H

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdint.h>
#include <netinet/in.h>
#include <cstdio>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>


class server {

public:
    server(char* address, uint16_t port);

    ~server() {
        close(listener);
    }

    int run();

private:
    sockaddr_in addr;
    int listener;
    const size_t BUFFER_SIZE = 1024;
};


#endif //HW5_NET_SERVER_H
