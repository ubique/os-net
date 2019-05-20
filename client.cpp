#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <cstddef>
#include <cstring>
#include <iostream>

#include "client.h"

client::client(const std::string &address, const std::string &port) {
    create_socket();
    set_sock_options(address, port);
}

void client::connect_to_server() {
    if (connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) <
            0) {
        print_fatal_error("Connection Failed");
    }
}

void client::work() {
    const std::string hello = "hello, server";
    send(client_fd, hello.c_str(), hello.size(), 0);
    std::cout << "client: sent" << std::endl;

    std::array<char, 1024> buffer;
    read(client_fd, buffer.data(), buffer.size());
    std::cout << buffer.data() << std::endl;
}

void client::create_socket() {
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
    }
}

void client::set_sock_options(const std::string &addr_to_connect,
                              const std::string &port) {
    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(port.c_str()));

    if (inet_pton(AF_INET, addr_to_connect.c_str(), &serv_addr.sin_addr) <= 0) {
        std::cerr << "client: invalid ipv4 address" << std::endl;
        throw std::runtime_error("invalid ipv4 address");
    }
}

void client::print_fatal_error(const std::string &err) {
    perror(err.c_str());
    throw std::runtime_error("client: " + err);
}
