#include <arpa/inet.h>
#include "server.hpp"

tftp_server::tftp_server() : socket_desc(), server_address{0} {
    memset(&server_address, 0, sizeof(sockaddr_in));

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(socket_desc.get_descriptor(), reinterpret_cast<sockaddr*>(&server_address), sizeof(sockaddr_in)) == -1) {
        throw std::runtime_error("Failed to run server with socket connection");
    }
    logger().success("Server binded");
}

tftp_server::~tftp_server() {
    server_address = sockaddr_in{0};
    socket_desc.close();
}

[[ noreturn ]] void tftp_server::await() {
    while (true) {
        sockaddr_in client_address;
        memset(&client_address, 0, sizeof(sockaddr_in));
        char *buf = reinterpret_cast<char *>(malloc(30));
        ssize_t n;
        socklen_t len;
        n = recvfrom(socket_desc.get_descriptor(), reinterpret_cast<void *>(buf), 30, 0,
                     reinterpret_cast<sockaddr *>(&client_address), &len);
        if (n != -1) {
            buf[n] = '\0';
            logger().success("Received '" + std::string(buf) + "' of size " + std::to_string(n));
        }
    }
}

void tftp_server::respond() {

}

int main() {
    tftp_server server;
    server.await();
}