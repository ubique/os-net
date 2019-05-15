#include "POP3Server.h"
#include <iostream>
POP3Server::POP3Server(const std::string &host_name, int port = 110) {
    struct hostent *server;

    socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_fd < 0) {
        print_error("ERROR opening socket\n");
    }

    server = gethostbyname(host_name.c_str());
    if (server == nullptr) {
        print_error("ERROR, no such host\n");
        close(socket_fd);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy(reinterpret_cast<char*>(&server_addr.sin_addr.s_addr), static_cast<char*>(server->h_addr), server->h_length);
    server_addr.sin_port = htons(port);

    if (bind(socket_fd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == -1) {
        print_error("ERROR bind a socket\n");
        close(socket_fd);
    }

    if (listen(socket_fd, 16) == -1) {
        print_error("ERROR listen socket\n");
        close(socket_fd);
    }
}

void POP3Server::print_error(const std::string &msg) {
    perror(msg.c_str());
    exit(EXIT_FAILURE);
}


int POP3Server::run() {
    while(true) {
        sockaddr_in client{};
        socklen_t client_len;
        int cur_fd = accept(socket_fd, reinterpret_cast<sockaddr*>(&client), &client_len);
        if (cur_fd != -1) {
            size_t buf_len = 4096;
            char buf[buf_len];
            size_t msg_len;
            memset(buf, 0, buf_len);
            msg_len = recv(cur_fd, &buf, buf_len, 0);
            if (msg_len != -1) {
                if (strncmp(buf, "exit", msg_len) == 0) {
                    break;
                }
                std::cout << msg_len << std::endl;
                char* msg_to_client = "hello";
                if (send(cur_fd, msg_to_client, 5, 0) == -1) {
                    print_error("Error in send msg to client!\n");
                    close(cur_fd);
                    close(socket_fd);
                }
            } else {
                close(cur_fd);
                close(socket_fd);
                break;
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

