//
// Created by Михаил Терентьев on 2019-05-13.
//

#ifndef OS_NET_CLIENT_H
#define OS_NET_CLIENT_H

#include <cstdint>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <zconf.h>

using namespace std;

struct wrapper {
    explicit wrapper(int discriptor) : discriptor(discriptor) {}


    ~wrapper() {
        close(discriptor);
    }

    wrapper(wrapper const &) = delete;

    wrapper &operator=(int fd) {
        this->discriptor = fd;
        return *this;
    }

    wrapper &operator=(wrapper const &) = delete;

    bool isBroken() {
        return discriptor == -1;
    }

    int const &getDiscriptor() {
        return discriptor;
    }


private:
    int discriptor;
};

class client {
public:
    client(char *address, uint16_t port);

    string request(std::string text);

private:
    static const size_t BF_SZ = 1024;
    sockaddr_in addr_socket;
    wrapper d_socket;
};


#endif //OS_NET_CLIENT_H
