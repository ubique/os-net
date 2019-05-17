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

void execute(int sock) {
    char buf[BUFFER_SIZE];
    ssize_t bytes = recv(sock, buf, BUFFER_SIZE, 0);
    buf[bytes] = '\0';
    send(sock, buf, bytes, 0);
    close(sock);
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
    listen(listener, 1);
    for (int i = 0; i < tries; ++i) {
        int sock = accepting(listener);
        execute(sock);
    }
    close(listener);
    return 0;
}