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

client::client(char* sock_name)
{
    memset(&address, 0, sizeof(sockaddr_un));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, sock_name, sizeof(address.sun_path) - 1);
}

std::string client::sendAndReceive(const std::string& message) {
    char buffer[BUFFER_SIZE + 1];
    size_t ptrIn = 0;
    std::string result;

    while (ptrIn < message.size()) {
        int data_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
        if (data_socket == -1) {
            error("Client: unable to create data socket");
        }

        int ret;
        ret = connect(data_socket, reinterpret_cast<sockaddr*>(&address), sizeof(sockaddr));
        if (ret == -1) {
            error("Client: unable to connect to server");
        }

        size_t len = std::min(BUFFER_SIZE, message.size() - ptrIn);
        ret = write(data_socket, message.c_str() + ptrIn, len);
        if (ret == -1) {
            error("Client: unable to send data");
        }
        std::string request = message.substr(ptrIn, ret);
        std::cout << "Client sent:\n" << request << std::endl;
        ptrIn += ret;

        int ptrOut = 0;
        while (ptrOut < request.size()) {
            len = std::min(BUFFER_SIZE, message.size() - ptrOut);

            ret = read(data_socket, buffer, len);
            buffer[BUFFER_SIZE] = 0;
            if (ret == -1) {
                error("Client: unable to receive data");
            }
            std::string response(buffer, buffer + ret);
            std::cout << "Client received:\n" << response << "\n" <<  std::endl;

            ptrOut += ret;
            result += response;
        }

        close(data_socket);
    }
    return result;
}
