#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>

#include <string>
#include <iostream>

#include "Client.h"

Client::Client(uint16_t port) : port(port), cfd(socket(AF_INET, SOCK_STREAM, 0)) {

}

Client::~Client() {
    close(cfd);
}

void Client::run() {
    sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // TODO
    if (connect(cfd, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
        // TODO
    }

    char buf[1024];
    ssize_t received = 0;
    if ((received = recv(cfd, (char*)&buf, 1024, 0)) != -1) {
        std::cout << received << std::endl;
        for (size_t i = 0; i < received; i++)
            std::cout << buf[i];

        std::cout << std::endl;
    } else {

    }



}

void Client::sendRequest(std::string const& data) {
    if (send(cfd, data.data(), data.length(), 0) == -1) {
        std::cout << "ERROR: Request was not sent" << std::endl;
    }
}

std::string Client::getResponse() {
    ssize_t received;
    if ((received = recv(cfd, buffer, BUFFER_SIZE, 0)) != -1) {
        return std::string(buffer, received);
    } else {
        return "ERROR: Response was not received";
    }
}