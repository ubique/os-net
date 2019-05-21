#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#include <string>
#include <iostream>

#include "Client.h"

Client::Client(uint16_t port) : port(port), sfd(socket(AF_INET, SOCK_STREAM, 0)) {
    if (sfd == -1) {
        throw ClientException("Socket was not created");
    }
}

Client::~Client() {
    close(sfd);
}

void Client::run() {
    sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (connect(sfd, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
        throw ClientException("Connection failed");
    }

    auto greeting = getResponse();
    if (greeting[0] != '2') {
        throw ClientException("Server did not send greeting with 2** code :(");
    }
    std::cout << greeting;
}

void Client::sendRequest(std::string const& data) {
    if (send(sfd, data.data(), data.length(), 0) == -1) {
        std::cout << "ERROR: Request was not sent" << std::endl;
    }
}

std::string Client::getResponse() {
    ssize_t received;
    if ((received = recv(sfd, buffer, BUFFER_SIZE, 0)) != -1) {
        return std::string(buffer, received);
    } else {
        return "ERROR: Response was not received";
    }
}