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
    } else if (pton_status == -1) {
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
        client_socket.set(accept(socket_fd, reinterpret_cast<sockaddr*>(&client), &sz));

        if (client_socket == -1) {
            cerr << get_error_message("Could not accept connection") << endl;
            continue;
        }

        state = State::AUTH;
        std::string greeting = "+OK Simple synchronous POP3 server\r\n";
        write(client_socket, greeting.c_str(), greeting.size());

        while (state != State::WAIT && state != State::UPDATE) {
            memset(buffer, 0, 512);
            ssize_t bytes_read = read(client_socket, buffer, 512);
            if (bytes_read == -1) {
                cerr << get_error_message("Could not read data from client") << endl;
                state = State::WAIT;
                break;
            } else if (bytes_read == 512) {
                send_ERR("Command is too long");
                state = State::WAIT;
            }
            process_command(buffer);
        }

        client_socket.close();
    }
#pragma clang diagnostic pop
}

void POP_server::process_command(std::string command_line) {
    auto pos = command_line.find(' ');
    auto command = command_line.substr(0, pos);
    auto argument = (pos == std::string::npos) ? "" : command_line.substr(pos);
    while (!command.empty() && isspace(command.back())) {
        command.pop_back();
    }
    while (!argument.empty() && isspace(argument.back())) {
        argument.pop_back();
    }

    cerr << command << ":" << argument << endl;

    if (state == State::AUTH) {
        if (command == "user") {
            if (current_user.empty()) {
                current_user = argument;
                send_OK();
            } else {
                current_user.clear();
                send_ERR("User name was already sent");
            }
        } else if (command == "pass") {
            //TODO check login/password
            if (current_user.empty()) {
                send_ERR("User name must be sent first");
            } else {
                state = State::TRANSACTION;
                send_OK();
            }
        } else if (command == "quit") {
            state = State::WAIT;
            send_OK();
        } else {
            send_ERR("Illegal command");
        }
    } else if (state == State::TRANSACTION) {
        //TODO
        send_ERR("");
        state = State::WAIT;
    } else {
        throw std::logic_error("When processing commands state must be AUTH or TRANSACTION");
    }
}

void POP_server::send_OK(std::string const& message) {
    std::string response = "+OK " + message + "\r\n";
    send_all(response.c_str(), response.size());
}

void POP_server::send_OK() {
    send_OK("");
}

void POP_server::send_ERR(std::string const& message) {
    std::string response = "-ERR " + message + "\r\n";
    send_all(response.c_str(), response.size());
}

/**
 * Tries to send all data
 * If fails sets state to WAIT
 * @param data
 * @param size
 */
//THINK throw exception instead of setting state
void POP_server::send_all(const char *data, size_t size) {
    auto ptr = data;
    do {
        ssize_t written = write(client_socket, ptr, size);
        if (written == -1) {
            cerr << get_error_message("Could not write to socket") << endl;
            state = State::WAIT;
            return;
        }
        size -= written;
        ptr += written;
    } while (size > 0);
}