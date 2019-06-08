//
// Created by max on 07.05.19.
//

#include <unistd.h>
#include <stdexcept>
#include "Socket.h"
#include "helper.h"

using Symb = Printer::Symbols;

Socket::Socket() = default;

Socket::Socket(int fd) {
    data_socket = fd;
}

Socket::Socket(Socket &&soc) noexcept : connection_socket(soc.connection_socket), data_socket(soc.data_socket),
                                        m_flag(soc.m_flag), prev_data_socket(soc.prev_data_socket),
                                        soc_name(std::move(soc.soc_name)) {
    soc.m_flag = FLAG_DOWN;
    memcpy(&addr, &soc.addr, sizeof(struct sockaddr_un));
}

Socket &Socket::operator=(Socket &&soc) noexcept {
    if (m_flag != FLAG_DOWN) {
        close();
    }
    connection_socket = soc.connection_socket;
    data_socket = soc.data_socket;
    m_flag = soc.m_flag;
    prev_data_socket = soc.prev_data_socket;
    soc_name = std::move(soc.soc_name);
    soc.m_flag = FLAG_DOWN;
    memcpy(&addr, &soc.addr, sizeof(struct sockaddr_un));
    return *this;
}

void Socket::create(std::string m_soc_name, int flag) {
    Printer::print(std::cout, "Socket::create", Symb::End);
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
    Printer::print(std::cout, "Socket::bind", Symb::End);
    struct sockaddr_un name{};
    memset(&name, 0, sizeof(struct sockaddr_un));

    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, soc_name.data(), sizeof(name.sun_path) - 1);
    int bind_ret = ::bind(connection_socket, reinterpret_cast<const struct sockaddr *>(&name),
                          sizeof(struct sockaddr_un));
    checker(bind_ret, "Unable to bind");
}

Socket::~Socket() {
    if (m_flag != FLAG_DOWN)
        try {
            close();
        } catch (...) {
            std::cerr << "Fail" << std::endl;
        }
}

void Socket::connect() {
    Printer::print(std::cout, "Socket::connect", Symb::End);
    memset(&addr, 0, sizeof(struct sockaddr_un));

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, soc_name.data(), sizeof(addr.sun_path) - 1);

    checker(::connect(data_socket, (const struct sockaddr *) &addr,
                      sizeof(struct sockaddr_un)), "The server is down");
}

int Socket::accept() {
    Printer::print(std::cout, "Socket::accept", Symb::End);
    if (prev_data_socket != -1) {
        checker(::close(prev_data_socket), "Unable to close data socket");
    }
    data_socket = ::accept(connection_socket, nullptr, nullptr);
    checker(data_socket, "Unable to accept connection");
    prev_data_socket = data_socket;
    return data_socket;
}

std::pair<ssize_t, std::unique_ptr<char[]>> Socket::read() {
    Printer::print(std::cout, "Socket::read", Symb::End);
    std::unique_ptr<char[]> buffer(new char[BUFFER_SIZE + 1]);
    memset(buffer.get(), 1, sizeof(char) * (BUFFER_SIZE));
    ssize_t read_size = 0;
    while (read_size < BUFFER_SIZE) {
        ssize_t bytes_read = ::read(data_socket, buffer.get() + read_size, BUFFER_SIZE - read_size);
        checker(bytes_read, "Error while reading");
        read_size += bytes_read;
        if (buffer[read_size - 1] == '\0') {
            break;
        }
    }
    buffer[BUFFER_SIZE] = 0;
    return std::make_pair(read_size, std::move(buffer));
}

void Socket::write(const std::string &data) {
    Printer::print(std::cout, "Socket::write", Symb::End);
    ssize_t written = 0;
    while ((size_t) written != std::min(BUFFER_SIZE, data.length() + 1)) {
        ssize_t ret_write = ::write(data_socket, data.data() + written,
                                    std::min(BUFFER_SIZE, data.length() + 1) - written);
        checker(ret_write, "Unable to write");
        written += ret_write;
    }
}

void Socket::close() {
    Printer::print(std::cout, "Socket::close", Symb::End);
    if (m_flag == Socket::FLAG_SERVER) {
        checker(::close(connection_socket), "Unable to close connection socket");
        if (prev_data_socket != -1 && data_socket != -1) {
            ::close(data_socket);
        }
        checker(::unlink(soc_name.data()), "Unable to unlink");
    }
    std::cout << "Socket down" << std::endl;
}

void Socket::listen() {
    Printer::print(std::cout, "Socket::listen", Symb::End);
    int ret = ::listen(connection_socket, 100);
    checker(ret, "Unable to listen to the socket");
}
