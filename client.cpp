//
// Created by SP4RK on 18/05/2019.
//
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <netinet/in.h>
#include <cstring>

int connect(int port) {
    int openedSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (openedSocket < 0) {
        std::cerr << "Error while opening socket " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    int option = 1;
    int status;
    status = setsockopt(openedSocket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    if (status == -1) {
        std::cerr << "Error while setting option to socket " << strerror(errno) << std::endl;
    }

    sockaddr_in address{.sin_family = AF_INET, .sin_port = htons(port), .sin_addr.s_addr = htonl(INADDR_LOOPBACK)
    };
    status = connect(openedSocket, (struct sockaddr*) &address, sizeof(address));
    if (status == -1) {
        std::cerr << "Error while connecting to socket " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    return openedSocket;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "You are expected to enter 2 arguments: <port> <message>" << std::endl;
        exit(EXIT_FAILURE);
    }
    int connectedSocket = connect(atoi(argv[1]));
    std::string requestMessage = argv[2];
    send(connectedSocket, requestMessage.data(), requestMessage.length(), 0);
    char replyBuffer[1024];
    ssize_t status = recv(connectedSocket, &replyBuffer, sizeof(replyBuffer), 0);
    if (status == -1) {
        std::cerr << "Error while reading receiving message from socket" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Reply: \n" << replyBuffer << std::endl;
    close(connectedSocket);
}
