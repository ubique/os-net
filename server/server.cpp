//
// Created by vitalya on 19.05.19.
//

#include "server.h"

#include <stdexcept>
#include <cstring>
#include <zconf.h>
#include <iostream>

void error(const std::string &msg) {
    throw std::runtime_error(msg + ": " + strerror(errno));
}

const size_t server::BUFFER_SIZE = 1024;

server::server(char* socket_name)
    : connection_socket(socket(AF_UNIX, SOCK_SEQPACKET, 0))
    , sock_name(socket_name)
{
    if (connection_socket == -1) {
        error("Server: unable to create connection socket");
    }

    memset(&address, 0, sizeof(sockaddr_un));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, socket_name, sizeof(address.sun_path) - 1);

    int ret;
    ret = bind(connection_socket, reinterpret_cast<sockaddr*>(&address), sizeof(sockaddr));
    if (ret == -1) {
        error("Server: unable to bind connection socket");
    }

    ret = listen(connection_socket, 10);
    if (ret == -1) {
        error("Server: unable to listen connection socket");
    }
}

void server::start() {
    char buffer[BUFFER_SIZE + 1];
    std::cout << "Echo server started!\n" << std::endl;

    while (true) {
        int data_socket = accept(connection_socket, nullptr, nullptr);
        if (data_socket == -1) {
            error("Server: unable to create data socket");
            break;
        }

        int ret, len;
        len = read(data_socket, buffer, BUFFER_SIZE);
        if (len == -1) {
            error("Server: unable to receive data");
            continue;
        }
        buffer[BUFFER_SIZE] = 0;
        std::string message(buffer, buffer + len);
        std::cout << "Server received:\n" << message << std::endl;

        ret = write(data_socket, buffer, len);
        if (ret == -1) {
            error("Server: unable to send data");
        }
        std::cout << "Server sent:\n" << message << "\n" << std::endl;

        if (!strncmp(buffer, "SHUTDOWN", 8)) {
            down_flag = true;
        }

        close(data_socket);
        if (down_flag) {
            std::cout << "Server down." << std::endl;
            break;
        }
    }
}

server::~server() {
    close(connection_socket);
    unlink(sock_name.c_str());
}
