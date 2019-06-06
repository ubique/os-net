#include "client.h"

client::client(char *address, uint16_t port) {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        throw std::runtime_error("Can't init socket");
    }
    addr.sin_family = AF_INET;
    addr.sin_port = port;
    addr.sin_addr.s_addr = inet_addr(address);
//    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("connect");
        throw std::runtime_error("Can't make connection");
    }
}

std::string client::make_request(std::string request) {
    ssize_t all_sended = 0;
    while (all_sended < std::min(BUFFER_SIZE, request.length() + 1)) {
        ssize_t sended = send(sock, request.data() + all_sended, std::min(BUFFER_SIZE, request.length() + 1), 0);
        all_sended += sended;
        if (sended == -1) {
            perror("send");
            throw std::runtime_error("Can't send request");;
        }
    }

    std::vector<char> response(BUFFER_SIZE);
    ssize_t all_received = 0;
    while (all_received < BUFFER_SIZE) {
        ssize_t received = recv(sock, response.data() + all_received, BUFFER_SIZE, 0);
        all_received += received;
        if (received == -1) {
            perror("recv");
            throw std::runtime_error("Can't receive response");
        }
        if (response.back() == '\0') {
            break;
        }
    }

    response.resize(all_received);

    return response.data();
}