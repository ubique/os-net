//
// Created by daniil on 19.05.19.
//

#include <cstring>
#include <arpa/inet.h>
#include <vector>
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
    if (send(socket, text.c_str(), text.length(), 0) != text.length()) {
        throw client_exception("Сan't send request");
    }
    std::vector<char> response(2048);
    int read = recv(socket, response.data(), 2048, 0);
    if (read == -1) {
        throw client_exception("Can't read from file descriptor");
    }
    response.resize(read);
    return std::string(response.data());
}

client_exception::client_exception(const std::string &message) : std::runtime_error(message) {}
