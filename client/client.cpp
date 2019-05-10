#include "client.h"

client_exception::client_exception(std::string cause)
    : std::runtime_error(cause + ": " + strerror(errno)) {}

client::client(char* address, uint16_t port) : socket_fd(-1) {
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
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

std::string client::request(std::string text) {
    ssize_t text_length = static_cast<ssize_t>(text.length());
    if (send(socket_fd.value(), text.c_str(), text.length(), 0) !=
        text_length) {
        throw client_exception("Unable to send request");
    }

    std::vector<char> response(BUFFER_SIZE);
    ssize_t was_read = recv(socket_fd.value(), response.data(), BUFFER_SIZE, 0);
    if (was_read == -1) {
        throw client_exception("Unable to read from file descriptor");
    }
    response.resize(was_read);

    return std::string(response.data());
}
