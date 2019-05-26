#include "client.hpp"
#include "utils/logger.hpp"

#include <arpa/inet.h>

// @formatter:off
std::string const USAGE = "Simple ECHO client\n"
                          "Usage: ./client\n"
                          "After run:\n"
                          "\t- " + logger()._HELP + "HELP" + logger()._DEFAULT + "\n"
                          "\t\t to show help message\n"
                          "\t- " + logger()._HELP + "CONN" + logger()._DEFAULT + " address port\n"
                          "\t\t to establish connection with the server\n"
                          "\t- " + logger()._HELP + "ECHO" + logger()._DEFAULT + " message\n"
                          "\t\t to send message to connected server\n"
                          "\t- " + logger()._HELP + "EXIT" + logger()._DEFAULT + "\n"
                          "\t\t to stop client";
// @formatter:on

unsigned int const client::REPEAT = 100;

client::client() : socket_desc(), server_address{0} {}

client::~client() = default;

void client::connect(std::string const &address, uint16_t port) {
    memset(&server_address, 0, sizeof(sockaddr_in));

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(address.c_str());

    if (::connect(socket_desc.get_descriptor(),
                  reinterpret_cast<sockaddr *>(&server_address), sizeof(sockaddr_in)) == -1) {
        logger().fail("Failed to open socket connection", errno);
        return;
    }
    logger().success("Established connection with " + address + ":" + std::to_string(port));
}

void client::send(std::string const &message) {
    for (int i = 0; i < REPEAT; i++) {
        if (sendto(socket_desc.get_descriptor(), message.c_str(), message.length(), 0,
                   reinterpret_cast<sockaddr *>(&server_address), sizeof(server_address)) != -1) {
            logger().success("Sent message '" + message + "'");
            receive();
            return;
        }
    }
    logger().fail("Failed to send a message", errno);
}

void client::receive() {
    ssize_t n = -1;
    char *buf = reinterpret_cast<char *>(malloc(2048));
    time_t start = time(0);
    while (n == -1 && difftime(start, time(0)) < 5) {
        socklen_t len;
        n = recvfrom(socket_desc.get_descriptor(), reinterpret_cast<void *>(buf), 2048, 0,
                     reinterpret_cast<sockaddr *>(&server_address), &len);
        if (n != -1) {
            buf[n] = '\0';
            logger().success("Received '" + std::string(buf) + "' back, size is " + std::to_string(n));
        }
    }
    if (n == -1) {
        logger().fail("Failed to receive a response message");
    }
}

void client::disconnect() {
    server_address = sockaddr_in{0};
    socket_desc.close();
}

int main(int argc, char *argv[]) {

    if (argc != 1) {
        logger().fail("Zero arguments expected");
        std::cout << USAGE << std::endl;
        return 0;
    }

    client client;

    std::string cmd;
    while (true) {
        std::cin >> cmd;
        if (cmd == "HELP") {
            std::cout << USAGE << std::endl;
        } else if (cmd == "CONN") {
            std::string address;
            uint16_t port;
            std::cin >> address >> port;
            client.connect(address, port);
        } else if (cmd == "ECHO") {
            std::string message;
            std::cin.get();
            std::getline(std::cin, message);
            client.send(message);
        } else if (cmd == "EXIT") {
            client.disconnect();
            break;
        }
    }

}
