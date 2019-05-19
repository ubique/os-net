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
    if (send(sock, request.data(), request.length(), 0) < 0) {
        perror("send");
        throw std::runtime_error("Can't send request");
    }
    std::vector<char> response(BUFFER_SIZE);
    ssize_t read = recv(sock, response.data(), BUFFER_SIZE, 0);
    if (read < 0) {
        perror("recv");
        throw std::runtime_error("Can't receive response");
    }
    response.resize(read);

    return response.data();
}