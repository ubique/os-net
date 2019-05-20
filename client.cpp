#include "client.hpp"
#include "utils/logger.hpp"

#include <arpa/inet.h>

client::client() : socket_desc(), server_address{0} {}

client::~client() = default;

void client::connect(std::string const &address, uint16_t port) {
    memset(&server_address, 0, sizeof(sockaddr_in));

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(address.c_str());

    if (::connect(socket_desc.get_descriptor(), reinterpret_cast<sockaddr*>(&server_address), sizeof(sockaddr_in)) == -1) {
        throw std::runtime_error("Failed to open socket connection");
    }
    logger().success("Established connection with " + address + ":" + std::to_string(port));
}

void client::send(std::string const &local_path, std::string const &remote_path) {
    for (int i = 0; i < 10; i++) {
        std::string msg = "Hello, world! no " + std::to_string(i);
        sendto(socket_desc.get_descriptor(), msg.c_str(), msg.length(), 0,
               reinterpret_cast<sockaddr *>(&server_address), sizeof(server_address));
        logger().success("Sent message '" + msg + "'");
        receive("", "");
    }
}

void client::receive(std::string const &path, std::string const &remote_path) {
    ssize_t n = -1;
    char *buf = reinterpret_cast<char *>(malloc(2048));
    while (n == -1) {
        socklen_t len;
        n = recvfrom(socket_desc.get_descriptor(), reinterpret_cast<void *>(buf), 2048, 0,
                     reinterpret_cast<sockaddr *>(&server_address), &len);
        if (n != -1) {
            buf[n] = '\0';
            logger().success("Received '" + std::string(buf) + "' back, size is " + std::to_string(n));
        }
    }
}

void client::disconnect() {
    server_address = sockaddr_in{0};
//    socket_desc.close();
}

int main() {
    client client;
    client.connect("127.0.0.1", 8080);
    client.send("", "");
    client.disconnect();
}
