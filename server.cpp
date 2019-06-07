#include "server.hpp"

#include <arpa/inet.h>
#include <inttypes.h>
#include <limits>

// @formatter:off
std::string const USAGE = "Simple ECHO server\n"
                          "Usage: ./server [address [port]]\n"
                          "\t- " + logger()._HELP + "address" + logger()._DEFAULT + " is 127.0.0.1\n"
                                                                                    "\t- " + logger()._HELP + "port" +
                          logger()._DEFAULT + " is 8007";
// @formatter:on

unsigned int const server::REPEAT = 100;
unsigned int const server::MAX_QUEUE = 8;
unsigned int const server::BUFFER_SIZE = 2048;

server::server(std::string const &address, uint16_t port) : socket_desc(), server_address{0} {
    if (!socket_desc.check_valid()) {
        logger().fail("Could not create a fd for socket", errno);
        return;
    }
    memset(&server_address, 0, sizeof(sockaddr_in));

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(address.c_str());

    if (bind(socket_desc.get_descriptor(), reinterpret_cast<sockaddr *>(&server_address), sizeof(sockaddr_in)) < 0) {
        logger().fail("Failed to run server with socket connection", errno);
        return;
    }
    if (listen(socket_desc.get_descriptor(), MAX_QUEUE) < 0) {
        logger().fail("Failed to set up a listening mode for connection", errno);
    }
    logger().success("Server bind and set up completed");
}

server::~server() {
    server_address = sockaddr_in{0};
    socket_desc.close();
}

[[noreturn]] void server::await_and_respond() {
    sockaddr_in client_address{0};
    socklen_t socket_len = sizeof(sockaddr_in);

    while (true) {
        memset(&client_address, 0, sizeof(sockaddr_in));
        socket_wrapper client_desc(
                accept(socket_desc.get_descriptor(), reinterpret_cast<sockaddr *>(&client_address), &socket_len));
        if (!client_desc.check_valid()) {
            logger().fail("Could not accept a connection", errno);
            continue;
        }

        char *buf = reinterpret_cast<char *>(malloc(BUFFER_SIZE));
        int offset = 0;
        ssize_t n = -1;
        while (true) {
            for (int i = 0; i < REPEAT && n == -1; i++) {
                n = recv(socket_desc.get_descriptor(), reinterpret_cast<void *>(buf + offset), BUFFER_SIZE - offset, 0);
            }
            if (n == -1) {
                logger().fail("Could not read message", errno);
                continue;
            }
            if (buf[n - 1] == '\n') {
                buf[n - 1] = '\0';
                logger().success("Received '" + std::string(buf) + "' of size " + std::to_string(n));
                respond(client_desc, buf, n - 1);
                break;
            } else {
                offset += n;
            }
        }
    }
}


void server::respond(socket_wrapper &client_desc, char *buf, ssize_t n) {
    char response[] = {' ', '-', 'd', 'e', ' ', 'g', 'o', 'z', 'a', 'r', 'u', '!', '\n'};
    for (int i = 0; n < BUFFER_SIZE; n++, i++) {
        buf[n] = response[i];
    }
    buf[n - 1] = '\n';
    for (unsigned int i = 0; i < REPEAT; i++) {
        if (send(socket_desc.get_descriptor(), buf, static_cast<size_t>(n), 0) > -1) {
            buf[n - 1] = '\0';
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