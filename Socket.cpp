//
// Created by max on 07.05.19.
//

#include <unistd.h>
#include <stdexcept>
#include "Socket.h"
#include "helper.h"


Socket::Socket() = default;

void Socket::create(std::string m_soc_name, int flag) {
    soc_name = std::move(m_soc_name);
    m_flag = flag;
    std::cout << "Connecting to : " << soc_name << std::endl;
    if (flag == FLAG_SERVER) {
        checker(connection_socket = ::socket(AF_UNIX, SOCK_SEQPACKET, 0), "Unable to create socket");
    }
    if (flag == FLAG_CLIENT) {
        checker(data_socket = ::socket(AF_UNIX, SOCK_SEQPACKET, 0), "Unable to create socket");
    }
}

void Socket::bind() {
    struct sockaddr_un name{};
    memset(&name, 0, sizeof(struct sockaddr_un));

    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, soc_name.data(), sizeof(name.sun_path) - 1);
    int bind_ret = ::bind(connection_socket, reinterpret_cast<const struct sockaddr *>(&name),
                          sizeof(struct sockaddr_un));
    checker(bind_ret, "Unable to bind");
}

Socket::~Socket() {
    try {
        close();
    } catch (...) {
        std::cerr << "Fail" << std::endl;
    }
}

void Socket::connect() {
    memset(&addr, 0, sizeof(struct sockaddr_un));

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, soc_name.data(), sizeof(addr.sun_path) - 1);

    checker(::connect(data_socket, (const struct sockaddr *) &addr,
                      sizeof(struct sockaddr_un)), "The server is down");
}

int Socket::accept() {
    if (prev_data_socket != -1) {
        checker(::close(prev_data_socket), "Unable to close data socket");
    }
    data_socket = ::accept(connection_socket, nullptr, nullptr);
    checker(data_socket, "Unable to accept connection");
    prev_data_socket = data_socket;
    return data_socket;
}

std::pair<ssize_t, std::unique_ptr<char[]>> Socket::read() {
    std::unique_ptr<char[]> buffer(new char[BUFFER_SIZE + 1]);
    ssize_t bytes_read = ::read(data_socket, buffer.get(), BUFFER_SIZE);
    checker(bytes_read, "Error while reading");
    buffer[BUFFER_SIZE] = 0;
    return std::make_pair(bytes_read, std::move(buffer));
}

void Socket::write(const std::string &data) {
    int ret_write = ::write(data_socket, data.data(), BUFFER_SIZE);
    checker(ret_write, "Unable to write");
}

void Socket::close() {
    if (m_flag == Socket::FLAG_SERVER) {
        checker(::close(connection_socket), "Unable to close connection socket");
        checker(::close(data_socket), "Unable to close data socket");
        checker(::unlink(soc_name.data()), "Unable to unlink");
    }
    std::cout << "Socket down" << std::endl;
}

void Socket::listen() {
    int ret = ::listen(connection_socket, 100);
    checker(ret, "Unable to listen to the socket");
}
