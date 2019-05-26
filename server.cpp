#include "server.hpp"

#include <arpa/inet.h>
#include <inttypes.h>
#include <limits>

// @formatter:off
std::string const USAGE = "Simple ECHO server\n"
                          "Usage: ./server [address [port]]\n"
                          "\t- " + logger()._HELP + "address" + logger()._DEFAULT + " is 127.0.0.1\n"
                          "\t- " + logger()._HELP + "port" + logger()._DEFAULT + " is 8007";
// @formatter:on

unsigned int const server::REPEAT = 100;

server::server(std::string const &address, uint16_t port) : socket_desc(), server_address{0} {
    memset(&server_address, 0, sizeof(sockaddr_in));

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(address.c_str());

    if (bind(socket_desc.get_descriptor(), reinterpret_cast<sockaddr *>(&server_address), sizeof(sockaddr_in)) == -1) {
        logger().fail("Failed to run server with socket connection", errno);
        return;
    }
    logger().success("Server bind completed");
}

server::~server() {
    server_address = sockaddr_in{0};
    socket_desc.close();
}

[[noreturn]] void server::await_and_respond() {
    sockaddr_in client_address{0};
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
    for (unsigned int i = 0; i < REPEAT; i++) {
        if (sendto(socket_desc.get_descriptor(), buf, static_cast<size_t>(n), 0,
                   reinterpret_cast<sockaddr *>(&client_address), len) != -1) {
            logger().success("Sent message '" + std::string(buf) + "' back");
            return;
        }
    }
    logger().fail("Unable to respond", errno);
}

int main(int argc, char *argv[]) {

    if (argc > 3) {
        logger().fail("Invalid amount of arguments given");
        std::cout << USAGE << std::endl;
        return 0;
    }

    std::string address = "127.0.0.1";
    uint16_t port = 8007;

    if (argc > 1) {
        address = argv[1];
    }
    if (argc > 2) {
        char *endptr;
        intmax_t _port = strtoimax(argv[2], &endptr, 10);
        if (errno == ERANGE || _port < 0 || _port > std::numeric_limits<uint16_t>::max()
            || endptr == argv[1] || *endptr != '\0') {
            return logger().fail("Invalid port number");
        }
        port = static_cast<uint16_t>(_port);
    }

    server server(address, port);
    server.await_and_respond();

}