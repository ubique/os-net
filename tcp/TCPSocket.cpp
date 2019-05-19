#include <utility>

//
// Created by ifkbhit on 07.05.19.
//

#include "TCPSocket.h"

net::tcp::TCPSocket::TCPSocket(std::string ipv4, const std::string& port) : ipv4(std::move(ipv4)), port(std::stoi(port)) {}

bool net::tcp::TCPSocket::create() {

    Utils::message("Creating socket for " + ipv4 + ":" + std::to_string(port));

    if ((fileDescriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        return Utils::fail("Cannot create socket (socket function)");
    }

    struct sockaddr_in sockaddrIn{};

    sockaddrIn.sin_family = AF_INET;
    sockaddrIn.sin_port = htons(port);

    int ipValidateAndConvert = inet_pton(AF_INET, ipv4.c_str(), &sockaddrIn.sin_addr);

    switch (ipValidateAndConvert) {
        case -1:
            return Utils::fail("Cannot convert ipv4 address (inet_pton function)");
        case 0:
            return Utils::error("Wrong ipv4 address " + ipv4);
        default:
            Utils::message("ipv4 is valid. Socket created.");
    }
    if (!afterCreate(sockaddrIn)) {
        return false;
    }
    Utils::message("Socket successfully created and set up");
    return true;
}



int net::tcp::TCPSocket::getFileDescriptor() {
    return fileDescriptor;
}
