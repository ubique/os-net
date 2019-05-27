#ifndef OS_NET_CLIENT_H
#define OS_NET_CLIENT_H

#include <iostream>
#include <errno.h>
#include <vector>
#include <cstring>
#include <arpa/inet.h>

#include "fd.h"

const std::string HELP = R"SEQ(Usage:
    -help                       - usage this program
    <address> <port>            - address port, default address = 127.0.0.1, port = 10005
    -exit                       - exit client
)SEQ";

const size_t BUF_SIZE = 1 << 11;

void error(std::string message, bool help = false, bool need_exit = false) {
    std::cerr << message << ". " << strerror(errno) << "." << std::endl;

    if (help) {
        std::cerr << HELP << std::endl;
    }

    if (need_exit) {
        exit(EXIT_FAILURE);
    }
}

struct client {
    explicit client(const char* addr, const char* port);

    void run();

  private:
    sockaddr_in socket_addr;
    fd_wrapper socket_client_fd;
};

int main(int argc, char* argv[]);

#endif // OS_NET_CLIENT_H
