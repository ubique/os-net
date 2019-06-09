#ifndef OS_NET_RAII_SOCKET_H
#define OS_NET_RAII_SOCKET_H
#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>
#include <stdexcept>
#include <unistd.h>
#include "utils.h"

class raii_socket {
private:
    int sockfd;

public:
    raii_socket() {
        sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (sockfd == -1) {
            throw_error("Couldn't open socket");
        }
    }
    raii_socket(int fd) : sockfd(fd) {}
    int get_fd() const {
        return sockfd;
    }
    ~raii_socket() {
        if (close(sockfd)) {
            throw_error("Couldn't close socket");
        }
    }

};

#endif //OS_NET_RAII_SOCKET_H
