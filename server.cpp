#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <errno.h>
#include <array>
#include <cstring>
#include <iostream>

#include "server.h"

server::server(const std::string &address, const std::string &port) {
    check_ipv4(address);
    create_socket();
    set_sock_options(port);
    bind_to_address();
}

void server::wait_client() {
    if (listen(server_fd, 10) < 0) {
        detach(server_fd);
        print_fatal_error("listen");
    }
}

void server::accept_connection() {
    if ((new_socket = accept(server_fd, (struct sockaddr *)&server_address,
                             (socklen_t *)&addrlen)) < 0) {
        print_fatal_error("accept");
    }
}

void server::work() {
    std::array<char, 1024> buffer;
    buffer.fill(0);
    const int flags = 0;

    int res = 1, revc_data_cnt = 0;
    while ((res = recv(new_socket, buffer.data() + revc_data_cnt, buffer.size() - revc_data_cnt - 1, 0)) > 0) {
        revc_data_cnt += res;
        if (buffer[revc_data_cnt] == '\0') {
            break;
        }
    }
    if (res < 0) {
        detach(server_fd);
        print_fatal_error("receiving failed", false);
    }
    std::cout << buffer.data() << std::endl;

    const std::string hello = "hello, client";
    res = 1, revc_data_cnt = 0;
    while (revc_data_cnt != hello.size() && res > 0) {
        res = send(new_socket, hello.data() + revc_data_cnt, hello.size() - revc_data_cnt, flags);
        revc_data_cnt += res;
    }
    if (res < 0) {
        detach(server_fd);
        print_fatal_error("sending failed", false);
    }
    std::cout << "server: respond has been sent" << std::endl;
}

void server::check_ipv4(const std::string &address) {
    struct sockaddr_in sa;
    if (inet_pton(AF_INET, address.c_str(), &(sa.sin_addr)) == 0) {
        print_fatal_error("invalid ipv4 address", false);
    }
}

void server::create_socket() {
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        print_fatal_error("socket failed");
    }
}

void server::set_sock_options(const std::string &port) {
    const int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        detach(server_fd);
        print_fatal_error("setsockopt");
    }
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(atoi(port.c_str()));
}

void server::bind_to_address() {
    if (bind(server_fd, (struct sockaddr *)&server_address,
             sizeof(server_address)) < 0) {
        detach(server_fd);
        print_fatal_error("bind failed");
    }
}

void server::detach(int fd) {
    if (close(fd) < 0) {
        perror("Can't close socket");
    }
}

void server::print_fatal_error(const std::string &err, bool perr) {
    throw std::runtime_error("server error: " + err +
                             (perr ? std::string(": ") + strerror(errno) : ""));
}
