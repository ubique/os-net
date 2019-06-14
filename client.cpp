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
        print_fatal_error("connection failed");
    }
}

void client::work(const std::string &msg) {
    if (msg.size() >= buffer_size) {
        detach(client_fd);
        print_fatal_error("too long message", false);
    }

    const int flags = 0;
    int res = 1, revc_data_cnt = 0;
    while (revc_data_cnt != msg.size() && res > 0) {
        res = send(client_fd, msg.data() + revc_data_cnt, msg.size() - revc_data_cnt, flags);
        revc_data_cnt += res;
    }
    if (res < 0) {
        detach(client_fd);
        print_fatal_error("sending failed", false);
    }
    std::cout << "client: greeting has been sent" << std::endl;

    std::array<char, buffer_size> buffer;
    buffer.fill(0);
    res = 1, revc_data_cnt = 0;
    while ((res = recv(client_fd, buffer.data() + revc_data_cnt, buffer.size() - revc_data_cnt - 1, 0)) > 0) {
        revc_data_cnt += res;
        if (buffer[revc_data_cnt] == '\0') {
            break;
        }
    }
    if (res < 0) {
        detach(client_fd);
        print_fatal_error("receiving failed", false);
    }
    std::cout << buffer.data() << std::endl;
}

void client::create_socket() {
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        print_fatal_error("socket failed");
    }
}

void client::set_sock_options(const std::string &addr_to_connect,
                              const std::string &port) {
    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(port.c_str()));

    if (inet_pton(AF_INET, addr_to_connect.c_str(), &serv_addr.sin_addr) <= 0) {
        print_fatal_error("invalid ipv4 address", false);
    }
}

void client::detach(int fd)
{
    if (close(fd) < 0) {
        perror("Can't close socket");
    }
}

void client::print_fatal_error(const std::string &err, bool perr) {
    throw std::runtime_error("client error: " + err +
                             (perr ? std::string(": ") + strerror(errno) : ""));
}
