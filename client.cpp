#include "client.hpp"
#include "server.hpp"
#include "utils/logger.hpp"

#include <arpa/inet.h>

// @formatter:off
std::string const USAGE = "Simple ECHO client\n"
                          "Usage: ./client\n"
                          "After run:\n"
                          "\t- " + logger()._HELP + "HELP" + logger()._DEFAULT + "\n"
                          "\t\t to show help message\n"
                          "\t- " + logger()._HELP + "CONN" + logger()._DEFAULT + " address port\n"
                          "\t\t to establish connection with the server to send ONE message\n"
                          "\t- " + logger()._HELP + "ECHO" + logger()._DEFAULT + " message\n"
                          "\t\t to send message to connected server\n"
                          "\t- " + logger()._HELP + "EXIT" + logger()._DEFAULT + "\n"
                          "\t\t to stop client";
// @formatter:on

unsigned int const client::REPEAT = 100;
unsigned int const client::BUFFER_SIZE = 2048;

client::client() : socket_desc(), server_address{0} {}

client::~client() = default;

void client::connect(std::string const &address, uint16_t port) {
    socket_desc.renew();
    memset(&server_address, 0, sizeof(sockaddr_in));

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(address.c_str());

    if (::connect(socket_desc.get_descriptor(),
                  reinterpret_cast<sockaddr *>(&server_address), sizeof(sockaddr_in)) < 0) {
        logger().fail("Failed to open socket connection", errno);
        return;
    }
    logger().success("Established connection with " + address + ":" + std::to_string(port));
}

void client::send(std::string const &message) {
    std::string msg = message + "\n";
    if (msg.size() > BUFFER_SIZE) {
        logger().fail("Too large message, unable to send");
        return;
    }
    socket_desc.send_message(msg.c_str(), msg.length(), REPEAT);
    char *buf = reinterpret_cast<char *>(malloc(BUFFER_SIZE));
    size_t buffer_size = 0;
    socket_desc.receive_message(buf, buffer_size, BUFFER_SIZE, REPEAT);
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
        } else {
            logger().fail("Unknown command, use HELP");
        }
    }

}
