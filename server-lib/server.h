//
// Created by Михаил Терентьев on 2019-05-13.
//

#ifndef OS_NET_SERVER_H
#define OS_NET_SERVER_H


#include <iostream>
#include <stdexcept>
#include <vector>

#include <arpa/inet.h>
#include <cstring>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

struct wrapper {
    explicit wrapper(int fd) : discriptor(fd) {}

    wrapper(wrapper const &) = delete;

    wrapper &operator=(wrapper const &) = delete;

    bool isBroken() {
        return discriptor == -1;
    }

    int const &getDiscriptor() {
        return discriptor;
    }

    ~wrapper() {
        close(discriptor);
    }

private:
    int discriptor;
};

struct server {
    server(char *address, uint16_t port);

    server(server const &) = delete;

    server &operator=(server const &) = delete;

    [[noreturn]] void run();

private:
    sockaddr_in saddress;
    wrapper d_socket;

    const size_t BF_SZ = 1024;

    void fillAddress(char *address, uint16_t port);
};

#endif //OS_NET_SERVER_H
