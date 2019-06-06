#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <cstring>

const int BUFFER_SIZE = 1024;

int open_socket() {
    int sock;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Can't create a socket: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "Can't set options on socket: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    return sock;
}

void connecting(int sock, int port) {
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        std::cerr << "Can't connect client socket: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
}

std::string get_request() {
    std::string request;
    std::cout << "Type your request" << std::endl;
    getline(std::cin, request);
    return request;
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
    int sock = open_socket();
    connecting(sock, port);
    bool executionError = false;
    for (int i = 0; i < tries; ++i) {
        std::string message = get_request();
        ssize_t sent = 0;
        while (sent != message.length()) {
            ssize_t delta = send(sock, message.data() + sent, message.length() - sent, 0);
            if (delta == -1) {
                std::cerr << "Can't send: " << std::strerror(errno) << std::endl;
                executionError = true;
                break;
            }
            if (delta == 0) {
                break;
            }
            sent += delta;
        }
        if (executionError) {
            break;
        }
        char server_reply[BUFFER_SIZE];
        std::cout << "Reply from server: " << std::endl;
        ssize_t received = 0;
        while (received != message.length()) {
            ssize_t delta = recv(sock, server_reply, BUFFER_SIZE, 0);
            if (delta == 0) {
                break;
            }
            if (delta == -1) {
                std::cerr << "Receive failed: " << std::strerror(errno) << std::endl;
                executionError = true;
                break;
            }
            for (int i = 0; i < delta; ++i) {
                std::cout << server_reply[i];
            }
            std::cout << std::endl;
            received += delta;
        }
        if (executionError) {
            break;
        }
    }
    if (close(sock) < 0) {
        std::cerr << "Can't close socket: " << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
