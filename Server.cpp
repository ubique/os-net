//
// Created by daniil on 18.05.19.
//

#include <cstring>
#include <arpa/inet.h>
#include <vector>
#include "Server.h"

Server::Server(char *serverAddress, uint16_t port) : socket(::socket(AF_INET, SOCK_STREAM, 0)) {
    if (socket == -1) {
        throw server_exception("Can't create socket");
    }
    memset(&address, 0, sizeof(sockaddr_in));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(serverAddress);
    address.sin_port = port;
    if (bind(socket, reinterpret_cast<sockaddr *>(&address), sizeof(sockaddr)) == -1) {
        throw server_exception("Can't bind socket");
    }
    if (listen(socket, 3) == -1) {
        throw server_exception("Can't listen socket");
    }
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

void Server::run() {
    while (true) {
        std::cout << "Waiting for connection.." << std::endl;
        sockaddr_in connectionAddr{};
        socklen_t connectionSize;
        int connection(accept(socket, reinterpret_cast<sockaddr *>(&connectionAddr), &connectionSize));
        if (connection == -1) {
            std::cerr << "Can't accept connection: " << strerror(errno) << std::endl;
            continue;
        }
        std::cout << "Connected" << std::endl;
        while (true) {
            std::vector<char> response(SIZE);
            size_t read_size = 0;
            while (read_size == 0 || response[read_size - 1] != '\0') {
                ssize_t cur_read_size = ::recv(connection, response.data() + read_size, SIZE, 0);
                if (cur_read_size == -1) {
                    throw server_exception("Error while reading");
                }
                read_size += cur_read_size;
            }
            response.resize(read_size);
            size_t write_size = 0;
            while (write_size != response.size()) {
                ssize_t cur_write_size = ::send(connection, response.data() + write_size, response.size() - write_size, 0);
                if (cur_write_size == -1) {
                    throw server_exception("Error while sending");
                }
                write_size += cur_write_size;
            }
        }
    }
}

#pragma clang diagnostic pop

server_exception::server_exception(const std::string &message) : std::runtime_error(message + strerror(errno)) {}
