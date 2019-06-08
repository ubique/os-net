//
// Created by daniil on 19.05.19.
//

#include <cstring>
#include <arpa/inet.h>
#include <vector>
#include <iostream>
#include "Client.h"

Client::Client(char *clientAddress, uint16_t port) : socket(::socket(AF_INET, SOCK_STREAM, 0)) {
    if (socket == -1) {
        throw client_exception("Can't create socket");
    }
    memset(&address, 0, sizeof(sockaddr));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(clientAddress);
    address.sin_port = port;

    socklen_t size = sizeof(sockaddr_in);
    if (connect(socket, reinterpret_cast<sockaddr *>(&address), size) == -1) {
        throw client_exception("Can't connect to server");
    }
}

std::string Client::request(const std::string& text) {
    size_t write_size = 0;
    while (write_size != text.length() + 1) {
        ssize_t cur_write_size = ::send(socket, text.c_str() + write_size, text.length() + 1 - write_size, 0);
        if (cur_write_size == -1) {
            throw client_exception("Error while sending");
        }
        write_size += cur_write_size;
    }

    std::vector<char> response(SIZE);
    size_t read_size = 0;
    while (read_size == 0 || response[read_size - 1] != '\0') {
        ssize_t cur_read_size = recv(socket, response.data() + read_size, SIZE, 0);
        if (cur_read_size == -1) {
            throw client_exception("Error while reading");
        }
        read_size += cur_read_size;
    }
    response.resize(read_size);
    return std::string(response.data());
}

client_exception::client_exception(const std::string &message) : std::runtime_error(message) {}
