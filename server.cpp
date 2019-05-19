//
// Created by damtev on 19.05.19.
//

#include "server.h"

server::server(char *address, uint16_t port) {
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener == -1) {
        perror("socket");
        throw std::runtime_error("Can't init socket");
    }

    addr.sin_family = AF_INET;
    addr.sin_port = port;
    addr.sin_addr.s_addr = inet_addr(address);
//    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (bind(listener, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("bind");
        throw std::runtime_error("Can't bind socket");
    }

    if (listen(listener, 1) < 0) {
        perror("listen");
        throw std::runtime_error("Error during listening");
    }

    std::cout << "Server started" << std::endl;
}

void server::run() {
    while (true) {
        int sock = accept(listener, nullptr, nullptr);
        if (sock < 0) {
            perror("accept");
            continue;
        }

        while (true) {
            std::vector<char> request(BUFFER_SIZE);
            ssize_t read = recv(sock, request.data(), BUFFER_SIZE, 0);
            if (read == -1) {
                perror("recv");
            } else if (read == 0) {
                std::cout << "Disconnection" << std::endl;
                break;
            } else {
                if (send(sock, request.data(), static_cast<size_t>(read), 0) < 0) {
                    perror("send");
                }
            }
        }

        close(sock);
    }
}
