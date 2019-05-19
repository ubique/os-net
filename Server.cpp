
#include <sys/socket.h>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

#include "Server.h"

Server::Server(char *address, uint16_t port) {
    openSocket();
    setUp(address, port);

}

void Server::openSocket() {
    mySocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mySocket < 0) {
        throw std::runtime_error("cannot open socket: " + std::string(strerror(errno)));
    }
}

void Server::setUp(char *address, uint16_t port) {
    sockaddr_in sockaddr{};
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = port;
    sockaddr.sin_addr.s_addr = inet_addr(address);
    if (bind(mySocket, (struct sockaddr *) &sockaddr, sizeof(sockaddr)) < 0) {
        throw std::runtime_error("cannot bind the socket: " + std::string(strerror(errno)));
    }

    if (listen(mySocket, 1) < 0) {
        throw std::runtime_error("cannot listen the socket: " + std::string(strerror(errno)));
    }

}

void Server::start() {
    int acceptSocket = accept(mySocket, nullptr, nullptr);
    if (acceptSocket < 0) {
        std::cerr << "cannot accept connection to the socket " << std::endl;
        return;
    }
    while (true) {
        char request[BUFFER_SIZE];
        ssize_t reqBytesNum = recv(acceptSocket, request, BUFFER_SIZE, 0);
        if (reqBytesNum == 0) {
            std::cout << "no more data from client" << std::endl;
            break;
        } else if (reqBytesNum < 0) {
            std::cerr << "cannot receive data from client" << std::endl;
            continue;
        }
        request[reqBytesNum] = '\0';
        std::cout << "got from client: " << request << std::endl;
        if (send(acceptSocket, request, static_cast<size_t >(reqBytesNum), 0) < 0) {
            std::cerr << "cannot send to client" << std::endl;
        } else {
            std::cout << "data sent to client" << std::endl;
        }
    }
}

Server::~Server() {
    close(mySocket);
}

