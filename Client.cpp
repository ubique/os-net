#include <sys/socket.h>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Client.h"

Client::Client(char *address, uint16_t port) {
    openSocket();
    makeConnection(address, port);

}

void Client::openSocket() {
    mySocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mySocket < 0) {
        throw std::runtime_error("cannot open socket: " + std::string(strerror(errno)));
    }
}

void Client::makeConnection(char *address, uint16_t port) {
    sockaddr_in sockaddr{};
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = port;
    sockaddr.sin_addr.s_addr = inet_addr(address);
    if (connect(mySocket, (struct sockaddr *) &sockaddr, sizeof(sockaddr)) < 0) {
        throw std::runtime_error("cannot connect to socket: " + std::string(strerror(errno)));
    }

}

std::string Client::makeRequest(std::string message) {
    send(mySocket, message.data(), message.length(), 0);
    char answer[BUFFER_SIZE];
    ssize_t reqByteNum = recv(mySocket, answer, BUFFER_SIZE, 0);
    if (reqByteNum < 0) {
        throw std::runtime_error("error while getting answer from server: " + std::string(strerror(errno)));
    }
    answer[reqByteNum] = '\0';
    return answer;
}

Client::~Client() {
    close(mySocket);
}

