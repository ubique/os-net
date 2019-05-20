//
// Created by roman on 20.05.19.
//

#include "hello_server.h"
#include "error.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <memory.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <chrono>

using std::runtime_error;
using std::cout;
using std::cerr;
using std::string;

hello_server::hello_server() {
    memset(buffer, 0, BUFFER_SIZE + 1);
    tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket == -1) {
        error("Socket cannot be created");
    }
}

void hello_server::start(const char *hostAddress, const in_port_t port) {
    struct sockaddr_in server{};
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;

    server.sin_port = port;

    if (inet_aton(hostAddress, &server.sin_addr) == 0) {
        error("Internet host address is invalid");
    }

    if (bind(tcp_socket, reinterpret_cast<sockaddr *>(&server), sizeof(sockaddr_in)) == -1) {
        error("Cannot bind");
    }

    if (listen(tcp_socket, 32) == -1) {
        error("Cannot start listening the socket");
    }
    while (true) {
        struct sockaddr_in user{};
        socklen_t socklen = sizeof(sockaddr_in);
        int client_fd = accept(tcp_socket, reinterpret_cast<sockaddr *>(&user), &socklen);
        if (client_fd == -1) {
            error("Cannot accept");
        }
        cout << "New user connected" << std::endl;
        while (true) {
            memset(buffer, 0, BUFFER_SIZE + 1);
            ssize_t readed = read(client_fd, buffer, BUFFER_SIZE);
            if (readed == -1) {
                cerr << "Error during reading request: " << strerror(errno) << std::endl;
                //do not drop server
                break;
            } else if (readed == 0) {
                cout << "User disconnected" << std::endl;
                break;
            } else {
                cout << "REQUEST: " + std::string(buffer) << std::endl;
                if (strcmp(buffer, "stop") == 0) {
                    break;
                }
                make_response();
                size_t response_len = sizeof(response_len) + readed - 1;

                usleep(1000000);
                if (send(client_fd, buffer, response_len, 0) != response_len) {
                    cerr << "cannot send request" << std::endl;
                    //do not drop server
                }
            }
        }
        if (close(client_fd) == -1) {
            cerr << "Cannot close connection" << std::endl;
            //do not drop server
        }
        if (strcmp(buffer, "stop") == 0) {
            if (close(tcp_socket) == -1) {
                error("Cannot close socket");
            }
            break;
        }
    }
}



void hello_server::make_response() {
    memcpy(buffer + sizeof(response_prefix) - 1, buffer, BUFFER_SIZE);
    memcpy(buffer, response_prefix, sizeof(response_prefix) - 1);
}



