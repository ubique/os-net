#include <strings.h>
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

int server::run() {
    while (true) {
        int sock = accept(listener, nullptr, nullptr);
        if (sock < 0) {
            perror("accept");
            continue;
        }

        while (true) {
            std::vector<char> request(BUFFER_SIZE);
            ssize_t all_received = 0;
            while (all_received < BUFFER_SIZE) {
                ssize_t received = recv(sock, request.data() + all_received, BUFFER_SIZE, 0);
                all_received += received;
                if (received == -1) {
                    perror("recv");
                    close(sock);
                    throw std::runtime_error("Can't receive request");
                }
                if (request.back() == '\0') {
                    break;
                }
            }

            if (strcasecmp(request.data(), "ex") == 0) {
                std::cout << "Exiting" << std::endl;
                break;
            }

            ssize_t all_sended = 0;
            while (all_sended < std::min(BUFFER_SIZE, request.size() + 1)) {
                ssize_t sended = send(sock, request.data() + all_sended, BUFFER_SIZE, 0);
                if (sended == -1) {
                    perror("send");
                    close(sock);
                    throw std::runtime_error("Can't send response");
                }
                all_sended += sended;
            }
        }

        close(sock);

        return 0;
    }
}
