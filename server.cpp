#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <fcntl.h>

const int BUFFER_SIZE = 1024;

int open_socket() {
    int listener;
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
        std::cerr << "Can't create a socket: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    int opt = 1;
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "Can't set options on socket: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    return listener;
}

void binding(int listener, int port) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(listener, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        std::cerr << "Error while binding: " << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
}

int accepting(int listener) {
    int sock = accept(listener, nullptr, nullptr);
    if (sock < 0) {
        std::cerr << "Can't accept: " << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    return sock;
}

int main(int argc, char **argv) {
    int port;
    int tries = 5;
    if (argc == 2) {
        port = atoi(argv[1]);
    } else {
        std::cerr << "Not enough arguments. One argument required: [port]" << std::endl;
        exit(EXIT_FAILURE);
    }
    int listener;
    listener = open_socket();
    binding(listener, port);
    if (listen(listener, 1) < 0) {
        std::cerr << "Error while listening: " << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < tries; ++i) {
        int sock = accepting(listener);
        char buf[BUFFER_SIZE];
        bool executionError = false;
        while (ssize_t received = recv(sock, buf, BUFFER_SIZE, 0)) {
            if (received == 0) {
                break;
            }
            if (received == -1) {
                std::cerr << "Receive failed: " << std::strerror(errno) << std::endl;
                break;
            }
            ssize_t sent = 0;
            while (sent != received) {
                ssize_t delta = send(sock, buf + sent, received - sent, 0);
                if (delta < 0) {
                    std::cerr << "Can't send: " << std::strerror(errno) << std::endl;
                    executionError = true;
                    break;
                }
                sent += delta;
            }
            if (executionError) {
                break;
            }
        }
        if (close(sock) < 0) {
            std::cerr << "Can't close socket: " << std::strerror(errno) << std::endl;
            break;
        }
    }
    if (close(listener) < 0) {
        std::cerr << "Can't close socket: " << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
