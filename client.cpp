#include "client.h"
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdexcept>

client::client(uint32_t address, in_port_t port) : address(address), port(port), sock(-1) {}

client::~client() {
    if (sock != - 1) {
        close(sock);
    }
}

void client::connect_to_server() {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        throw std::runtime_error("Error creating socket");
    }
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = port;
    addr.sin_addr.s_addr = address;

    if (connect(sock, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == -1) {
        throw std::runtime_error("Connection error");
    }
}

void client::echo(std::string &str) {
    if (sock == -1) {
        connect_to_server();
    }

    const char *message = str.c_str();
    size_t str_len = sizeof(message);

    if (send(sock, message, str_len, 0) != str_len) {
        throw std::runtime_error("Sending error");
    }

    char buf[BUFFER_SIZE];
    if (recv(sock, buf, BUFFER_SIZE, 0) == -1) {
        throw std::runtime_error("Reading error");
    }

    printf("%s\n", buf);
}