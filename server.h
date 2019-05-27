#ifndef OS_NET_SERVER_H
#define OS_NET_SERVER_H

#include <iostream>
#include <cstring>
#include <vector>
#include <errno.h>
#include <arpa/inet.h>

#include "fd.h"

const std::string HELP = R"SEQ(Usage:
    -help                       - usage this program
    <address> <port>			- address and port, default address = 127.0.0.1, port = 10005
)SEQ";

const size_t BUF_SIZE = 1 << 10;

void error(std::string message, bool with_errno = true, bool help = false, bool need_exit = false) {
    std::cerr << message << ". ";
    
    if (with_errno) {
        std::cerr << strerror(errno) << ".";
    }
    std::cerr << std::endl;

    if (help) {
        std::cerr << HELP << std::endl;
    }

    if (need_exit) {
        exit(EXIT_FAILURE);
    }
}

struct server {
    explicit server(const char* address, const char* port);

    void run();

private:
    sockaddr_in socket_addr;
    fd_wrapper socket_fd;
};

int main(int argc, char* argv[]);

#endif // OS_NET_SERVER_H
