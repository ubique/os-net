#include "server.h"

static void print_error(std::string const& cause) {
    std::cerr << cause << std::endl;
}

server_exception::server_exception(std::string cause)
    : std::runtime_error(cause + ": " + strerror(errno)) {}

server::server(char* address, uint16_t port)
    : socket_fd(socket(AF_INET, SOCK_STREAM, 0)) {
    if (socket_fd.bad()) {
        throw server_exception("Unable to create socket ");
    }
    memset(&server_address, 0, sizeof(sockaddr_in));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(address);
    server_address.sin_port = port;

    if (bind(socket_fd.value(), reinterpret_cast<sockaddr*>(&server_address),
             sizeof(sockaddr)) == -1) {
        throw server_exception("Unable to bind the socket");
    }

    if (listen(socket_fd.value(), BACKLOG) == -1) {
        throw server_exception("Unable to listen socket");
    }
}

[[noreturn]] void server::run() {
    while (true) {
        sockaddr_in peer;
        socklen_t peer_size;

        fd_wrapper infd(accept(socket_fd.value(),
                               reinterpret_cast<sockaddr*>(&peer), &peer_size));

        if (infd.bad()) {
            print_error("Cannot accept peer");
            continue;
        }

        std::vector<char> buffer(BUFFER_SIZE);
        ssize_t was_read = read(infd.value(), buffer.data(), BUFFER_SIZE);
        if (was_read == -1) {
            print_error("Cannot read file descriptor");
        } else {
            while (send(infd.value(), buffer.data(),
                        static_cast<size_t>(was_read), 0) != was_read) {
                print_error("Cannot send echo");
                std::cerr << "Retrying..." << std::endl;
            }
        }
    }
}
