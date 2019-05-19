#include "client.hpp"
#include "utils/logger.hpp"

#include <arpa/inet.h>

tftp_client::tftp_client() : socket_desc(), server_address{0} {}

tftp_client::~tftp_client() {
//    disconnect();
}

void tftp_client::connect(std::string const &address, uint16_t port) {
    memset(&server_address, 0, sizeof(sockaddr_in));

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(address.c_str());

    if (::connect(socket_desc.get_descriptor(), reinterpret_cast<sockaddr*>(&server_address), sizeof(sockaddr_in)) == -1) {
        throw std::runtime_error("Failed to open socket connection");
    }
    logger().success("Established connection with " + address + ":" + std::to_string(port));
}

void tftp_client::send(std::string const &local_path, std::string const &remote_path) {
    for (int i = 0; i < 10; i++) {
        std::string msg = "Hello, world! no " + std::to_string(i);
        sendto(socket_desc.get_descriptor(), msg.c_str(), msg.length(), 0,
               reinterpret_cast<sockaddr *>(&server_address), sizeof(server_address));
        logger().success("Sent message '" + msg + "'");
    }
}

void tftp_client::request(std::string const &path, std::string const &remote_path) {
    // skip for now
}

void tftp_client::disconnect() {
    server_address = sockaddr_in{0};
    socket_desc.close();
}

int main() {
    tftp_client client;
    client.connect("127.0.0.1", 8080);
    client.send("", "");
}
