#include "POP3Server.h"
#include <iostream>
#include <vector>
#include <Utils.h>
#include <DBase.h>

POP3Server::POP3Server(const std::string &host_name, int port = 110) {
    struct hostent *server;

    socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_fd < 0) {
        print_error("ERROR opening socket");
    }

    server = gethostbyname(host_name.c_str());
    if (server == nullptr) {
        print_error("ERROR, no such host");
        close(socket_fd);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy(reinterpret_cast<char*>(&server_addr.sin_addr.s_addr), static_cast<char*>(server->h_addr), server->h_length);
    server_addr.sin_port = htons(port);

    if (bind(socket_fd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == -1) {
        print_error("ERROR bind a socket");
        close(socket_fd);
    }

    if (listen(socket_fd, 16) == -1) {
        print_error("ERROR listen socket");
        close(socket_fd);
    }
}

void POP3Server::print_error(const std::string &msg) {
    perror(msg.c_str());
    exit(EXIT_FAILURE);
}


int POP3Server::run() {
    States state = AUTHORIZATION;
    DBase data_base;
    User user;
    while(true) {
        sockaddr_in client{};
        socklen_t client_len;
        int client_fd = accept(socket_fd, reinterpret_cast<sockaddr*>(&client), &client_len);
        if (client_fd != -1) {
            send_msg("+OK POP3 server ready", client_fd, "Error in send hello to client!");
            char buffer[BUFFER_LENGHT];
            memset(buffer, 0, BUFFER_LENGHT);
            size_t msg_len = recv(client_fd, &buffer, BUFFER_LENGHT, 0);
            if (msg_len == -1) {
                print_error("Error to receive a message");
                close(client_fd);
                close(socket_fd);
                break;
            }
            std::vector<std::string> request = Utils::split(buffer);
            if (request.size() != 2) {
                send_msg("-ERR", client_fd, "Error in send msg to client!");
            }
            if (state == AUTHORIZATION) {
                if (request[0] == "USER") {
                    if (data_base.is_user_by_login(request[1])) {
                        send_msg("+OK, user was found", client_fd, "Error in sending");
                        user = data_base.get_user_by_login(request[1]);
                    } else {
                        send_msg("-ERR, can't find user", client_fd, "Error in sending");
                    }
                } else if (request[0] == "PASS") {
                    if (user.get_login().empty()) {
                        send_msg("-ERR, haven't user session", client_fd, "Error in sending");
                    } else if (user.cmp_password(request[1])){
                        send_msg("+OK, authorization success", client_fd, "Error in sending");
                        state = TRANSACTION;
                    } else {
                        send_msg("-ERR, password is incorrect", client_fd, "Error in sending");
                    }
                } else {
                    send_msg("-ERR, unknown command", client_fd, "Error in sending");
                }
            } else if (state == TRANSACTION) {
                state = UPDATE;
            } else {
                state = AUTHORIZATION;
            }
        } else {
            print_error("Can't accept a client!");
            close(socket_fd);
            break;
        }

    }
}

POP3Server::~POP3Server() {
    if (close(socket_fd) == -1) {
        print_error("Can't close a socket\n");
    }
}

void POP3Server::send_msg(std::string msg, int fd, std::string msg_error) {
    if (send(fd, msg.c_str(), sizeof(msg.c_str()), 0) == -1) {
        print_error(msg_error);
        close(fd);
        close(socket_fd);
    }
}

