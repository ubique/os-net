//
// Created by roman on 20.05.19.
//

#include "server.h"
#include "my_error.h"
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

server::server(): socket_fd(socket(AF_INET, SOCK_STREAM, 0)) {
    memset(buffer, 0, BUFFER_SIZE + 1);
    if (socket_fd == -1) {
        my_error("Socket cannot be created");
        throw runtime_error("cannot create server");
    }
}

void server::start(const char *hostAddress, const in_port_t port) {
    struct sockaddr_in server{};
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;

    server.sin_port = port;

    if (inet_aton(hostAddress, &server.sin_addr) == 0) {
        my_error("Internet host address is invalid");
    }

    if (bind(socket_fd, reinterpret_cast<sockaddr *>(&server), sizeof(sockaddr_in)) == -1) {
        my_error("Cannot bind");
        return;
    }

    if (listen(socket_fd, 32) == -1) {
        my_error("Cannot start listening the socket");
        return;
    }
    while (true) {
        struct sockaddr_in user{};
        socklen_t socklen = sizeof(sockaddr_in);
        my_fd client_fd(accept(socket_fd, reinterpret_cast<sockaddr *>(&user), &socklen));
        if (client_fd == -1) {
            my_error("Cannot accept");
            return;
        }
        cout << "[INFO] New user connected" << std::endl;
        while (true) {
            memset(buffer, 0, BUFFER_SIZE);
            ssize_t readed = read(client_fd, buffer, BUFFER_SIZE);
            if (readed == -1) {
                my_error("Error during reading request");
                break; //do not drop server
            } else if (readed == 0) {
                break;
            } else {
                cout << "[REQUEST] " + std::string(buffer) << std::endl;
                if (strcmp(buffer, "stop") == 0) {
                    break;
                }
                usleep(100000); //0.1 sec
                if (send(client_fd, buffer, readed, 0) != readed) {
                    my_error("Cannot send response");
                    //do not drop server
                }
            }
        }
        cout << "[INFO] User disconnected" << std::endl;
        if (strcmp(buffer, "stop") == 0) {
            break;
        }
    }
}
