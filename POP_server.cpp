//
// Created by rsbat on 5/12/19.
//

#include "POP_server.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

using std::cerr;
using std::endl;


std::string get_error_message(const std::string &comment) {
    return comment + ": " + strerror(errno);
}

POP_server::POP_server(std::string address, std::string port) : state(State::WAIT), socket_fd(), current_user() {
    socket_fd.set(socket(AF_INET, SOCK_STREAM, 0));
    if (socket_fd == -1) {
        throw std::runtime_error(get_error_message("Could not create socket"));
    }

    struct sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(static_cast<uint16_t>(std::stoi(port)));

    int pton_status = inet_pton(AF_INET, address.c_str(), &serv_addr.sin_addr);
    if (pton_status == 0) {
        throw std::invalid_argument(get_error_message(address + " is not a valid address"));
    } else if (pton_status == 1) {
        throw std::domain_error(get_error_message("AF_INET is not supported"));
    }

    if (bind(socket_fd,
            reinterpret_cast<sockaddr*>(&serv_addr),
            sizeof(sockaddr_in)) == -1) {
        throw std::runtime_error(get_error_message("Could not bind socket"));
    }

    if (listen(socket_fd, MAX_BACKLOG) == -1) {
        throw std::runtime_error(get_error_message("Could not listen to socket"));
    }
}

[[noreturn]] void POP_server::run(){
    char buffer[512];

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    for(;;) {
        sockaddr_in client{};
        socklen_t sz = sizeof(sockaddr_in);
        Fd_wrapper client_socket{accept(socket_fd, reinterpret_cast<sockaddr*>(&client), &sz)};

        if (client_socket == -1) {
            cerr << get_error_message("Could not accept connection") << endl;
            continue;
        }

        state = State::AUTH;
        std::string greeting = "+OK Simple synchronous POP3 server\r\n";
        write(client_socket, greeting.c_str(), greeting.size());
    }
#pragma clang diagnostic pop
}
