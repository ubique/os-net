#include "server.h"
#include <sys/socket.h>
#include <cstdio>
#include <unistd.h>
#include <stdexcept>
#include "utils.h"

server::server(in_port_t port) : port(port), sock(-1) {}

server::~server() {
    if (sock != -1) {
        close(sock);
    }
}

void server::start() {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        throw std::runtime_error("Error creating socket");
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = port;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == -1) {
        throw std::runtime_error("Binding error");
    }

    if (listen(sock, 1) == -1) {
        throw std::runtime_error("Listening error");
    }

    while (true) {
        int temp_sock = accept(sock, nullptr, nullptr);
        if (temp_sock == -1) {
            perror("Accepting error");
            close(temp_sock);
            continue;
        }

        while (true) {
            auto read = socket_read(temp_sock);
            if (read.second == -1) {
                perror("Reading error");
                close(temp_sock);
                break;
            }
            if (read.second == 0) {
                break;
            }
            socket_send(temp_sock, read.first);
        }

        close(temp_sock);
    }
}