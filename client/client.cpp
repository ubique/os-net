//
// Created by vitalya on 19.05.19.
//

#include <cstring>
#include <stdexcept>
#include <zconf.h>
#include <iostream>
#include <cmath>
#include "client.h"

void error(const std::string &msg) {
    throw std::runtime_error(msg + ": " + strerror(errno));
}

const size_t client::BUFFER_SIZE = 1024;

client::client(char *socket_name)
    : sock_name(socket_name)
{
}

std::string client::send(const std::string& message) {
    data_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);

    if (data_socket == -1) {
        error("Client: unable to create data socket");
    }

    memset(&address, 0, sizeof(sockaddr_un));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, sock_name.c_str(), sizeof(address.sun_path) - 1);

    int ret;
    ret = connect(data_socket, reinterpret_cast<sockaddr*>(&address), sizeof(sockaddr));
    if (ret == -1) {
        error("Client: unable to connect to server");
    }

    char buffer[BUFFER_SIZE + 1];
    size_t ptr = 0;
    std::string result;

    while (ptr < message.size()) {
        size_t len = std::min(BUFFER_SIZE, message.size() - ptr);

        ret = write(data_socket, message.c_str() + ptr, len);
        if (ret == -1) {
            error("Client: unable to sent data");
        }
        std::cout << "Client sent:\n" << message << std::endl;

        ret = read(data_socket, buffer, BUFFER_SIZE);
        if (ret == -1) {
            error("Client: unable to receive data");
        }
        std::string response(buffer, buffer + ret);
        std::cout << "Client received:\n" << response << "\n" <<  std::endl;

        ptr += len;
        result += response;
    }
    return result;
}

client::~client() {
    close(data_socket);
}
