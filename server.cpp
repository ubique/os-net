#include <arpa/inet.h>
#include "server.hpp"

server::server() : socket_desc(), server_address{0} {
    memset(&server_address, 0, sizeof(sockaddr_in));

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(socket_desc.get_descriptor(), reinterpret_cast<sockaddr*>(&server_address), sizeof(sockaddr_in)) == -1) {
        throw std::runtime_error("Failed to run server with socket connection");
    }
    logger().success("Server binded");
}

server::~server() {
    server_address = sockaddr_in{0};
    socket_desc.close();
}

[[ noreturn ]] void server::await_and_respond() {
    sockaddr_in client_address{};
    while (true) {
        memset(&client_address, 0, sizeof(sockaddr_in));
        char *buf = reinterpret_cast<char *>(malloc(2048));
        ssize_t n;
        socklen_t len;
        n = recvfrom(socket_desc.get_descriptor(), reinterpret_cast<void *>(buf), 2048, 0,
                     reinterpret_cast<sockaddr *>(&client_address), &len);
        if (n != -1) {
            buf[n] = '\0';
            logger().success("Received '" + std::string(buf) + "' of size " + std::to_string(n));
            respond(client_address, buf, n, len);
        }
    }
}


void server::respond(sockaddr_in &client_address, char *buf, ssize_t n, socklen_t len) {
    sendto(socket_desc.get_descriptor(), buf, static_cast<size_t>(n), 0,
           reinterpret_cast<sockaddr*>(&client_address), len);
}

int main() {
    server server;
    server.await_and_respond();
}