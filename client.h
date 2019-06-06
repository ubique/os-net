#ifndef OS_NET_CLIENT_H
#define OS_NET_CLIENT_H

#include <iostream>
#include <errno.h>
#include <vector>
#include <cstring>
#include <arpa/inet.h>

#include "fd.h"

const std::string HELP = R"SEQ(Usage:
    You could send small message to server 
    -help                       - usage this program
    <address> <port>            - address port, default address = 127.0.0.1, port = 10005
    -exit                       - exit client
)SEQ";

const size_t BUF_SIZE = 1 << 11;
const ssize_t MAX_LEN_MESSAGE = 1 << 8 - 1;

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

// void error(std::string message, bool help = false, bool need_exit = false) {
//     std::cerr << message << ". " << strerror(errno) << "." << std::endl;

//     if (help) {
//         std::cerr << HELP << std::endl;
//     }

//     if (need_exit) {
//         exit(EXIT_FAILURE);
//     }
// }

struct client {
    explicit client(const char* addr, const char* port);

    void run();

  private:
    sockaddr_in socket_addr;
    fd_wrapper socket_client_fd;
};

int main(int argc, char* argv[]);

#endif // OS_NET_CLIENT_H
