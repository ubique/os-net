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
    if (argc == 2) {
        port = atoi(argv[1]);
    } else {
        std::cerr << "Not enough arguments. One argument required: [port]" << std::endl;
        exit(EXIT_FAILURE);
    }
    int sock = open_socket();
    connecting(sock, port);
    std::string message = get_request();
    send(sock, message.data(), message.length(), 0);
    char server_reply[BUFFER_SIZE];
    if (recv(sock, server_reply, BUFFER_SIZE, 0) < 0) {
        std::cerr << "recv failed" << std::endl;
    }
    std::cout << "Reply from server: " << std::endl;
    std::cout << server_reply << std::endl;
    close(sock);
    return 0;
}