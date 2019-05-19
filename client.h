#ifndef HW5_NET_CLIENT_H
#define HW5_NET_CLIENT_H

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdint.h>
#include <netinet/in.h>
#include <cstdio>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <vector>


class client {

public:
    client(char* address, uint16_t port);

    ~client() {
        close(sock);
    }

    std::string make_request(std::string request);

private:
    sockaddr_in addr;
    int sock;
    const size_t BUFFER_SIZE = 1024;
};


#endif //HW5_NET_CLIENT_H
