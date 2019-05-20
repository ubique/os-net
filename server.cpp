//
// Created by dumpling on 30.04.19.
//

#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

static const int LISTEN_BACKLOG = 50;
static const int BUF_SIZE = 4096;

static void print_err(const std::string &message) {
    std::cerr << "\033[31m" << message;
    if (errno) {
        std::cerr << ": " << std::strerror(errno);
    }
    std::cerr << "\033[0m" << std::endl;
}

int start(std::string &address, uint16_t port) {

    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {
        return -1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(address.c_str());

    if (bind(sfd, reinterpret_cast<sockaddr *>(&addr), sizeof(sockaddr_in)) == -1) {
        print_err("Bind failed: ");
        return -1;
    }

    return sfd;
}

int listen(int listener) {
    if (listen(listener, LISTEN_BACKLOG) == -1) {
        print_err("Bind failed: ");
        return -1;
    }

    while (true) {
        int exc = accept(listener, nullptr, nullptr);
        if (exc == -1) {
            return -1;
        }

        char buf[BUF_SIZE];
        int bytes_read;

        std::string message;
        while (true) {
            bytes_read = recv(exc, buf, BUF_SIZE, 0);

            if (bytes_read == -1) {
                break;
            }

            if (bytes_read == 0) {
                break;
            }

            for (int i = 0; i < bytes_read; ++i) {
                message += buf[i];
            }
        }
        close(exc);

        exc = accept(listener, nullptr, nullptr);
        auto m = message.c_str();
        int size = message.length();
        int total = 0;
        while (total < size) {
            int was_send = send(exc, m + total, size - total, 0);
            if (was_send == -1) {
                return -1;
            }

            total += was_send;
        }

        close(exc);
    }
}


int main(int argc, char **argv) {

    std::string address = "127.0.0.1";
    uint16_t port = 8005;

    int listener = start(address, port);
    if (listener != -1) {
        listen(listener);
    }
}

