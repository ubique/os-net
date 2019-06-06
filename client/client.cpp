#include "client.h"

client_exception::client_exception(std::string cause)
    : std::runtime_error(cause + ": " + strerror(errno)) {}

client::client(char* address, uint16_t port)
    : socket_fd(socket(AF_INET, SOCK_STREAM, 0)) {
    if (socket_fd.bad()) {
        throw client_exception("Unable to create socket");
    }
    memset(&server_address, 0, sizeof(sockaddr));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(address);
    server_address.sin_port = port;

    socklen_t addr_size = sizeof(sockaddr_in);
    if (connect(socket_fd.value(), reinterpret_cast<sockaddr*>(&server_address),
                addr_size) == -1) {
        throw client_exception("Unable to connect to server");
    }
}

std::string client::request(std::string text, accumulator& acc) {
    text += "\r\n";
    size_t was_sent = 0;
    while (was_sent < text.length()) {
        ssize_t current = send(socket_fd.value(), text.data() + was_sent,
                               text.length() - was_sent, 0);
        if (current == -1) {
            throw client_exception("Unable to send");
        }
        was_sent += static_cast<size_t>(current);
    }

    auto responce = acc.read_fully(socket_fd.value(), BUFFER_SIZE);
    if (responce.first == -1) {
        throw client_exception("Unable to read from file descriptor");
    }

    return responce.second;
}
