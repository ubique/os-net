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

server::server() : socket_fd(socket(AF_INET, SOCK_STREAM, 0)) {
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

            ssize_t readed = receive_request(client_fd);;
            if (readed == -1) {
//                my_error("Error during reading request");
                break; //do not drop server
            } else if (readed == 0) {
                break;
            } else {
                cout << "[REQUEST] " + std::string(buffer) << std::endl;
                if (strcmp(buffer, "stop") == 0) {
                    break;
                }
                usleep(100000); //0.1 sec
                response(client_fd, readed);
            }
        }
        cout << "[INFO] User disconnected" << std::endl;
        if (strcmp(buffer, "stop") == 0) {
            break;
        }
    }
}

void server::response(int client_fd, ssize_t count) {
    {
        ssize_t sent = 0;
        while (sent != sizeof(ssize_t)) {
            ssize_t written = write(client_fd, (char*)(&count) + sent, sizeof(ssize_t) - sent);
            if (written == -1) {
                my_error("Cannot response");
                return;
            }
            sent += written;
        }
    }
    {
        ssize_t sent = 0;
        while (sent != count) {
            ssize_t written = write(client_fd, buffer + sent, count - sent);
            if (written == -1) {
                my_error("Cannot response");
                return;
            }
            sent += written;
        }
    }
}

ssize_t server::receive_request(int client_fd) {
    ssize_t count = 0;
    {
        ssize_t received = 0;
        while (received != sizeof(ssize_t)) {
            ssize_t tmp = read(client_fd, (char*)(&count) + received, sizeof(ssize_t) - received);
            if (tmp == -1 || tmp == 0) {
                my_error("Cannot read request");
                return -1;
            }
            received += tmp;
        }
    }
    if (count == 0) {
        return 0;
    }
    {
        ssize_t received = 0;
        while (received != count) {
            ssize_t tmp = read(client_fd, buffer + received, count - received);
            if (tmp == -1 || tmp == 0) {
                my_error("Cannot response");
                return -1;
            }
            received += tmp;
        }
    }
    return count;
}
