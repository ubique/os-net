//
// Created by roman on 20.05.19.
//

#include <memory.h>
#include <arpa/inet.h>
#include "client.h"
#include "my_error.h"
#include <unistd.h>
#include <stdexcept>
#include <sys/types.h>
#include <sys/socket.h>

using std::runtime_error;
using std::string;

client::client(): socket_fd(socket(AF_INET, SOCK_STREAM, 0)) {
    if (socket_fd == -1) {
        my_error("Socket cannot be created");
        throw runtime_error("Cannot create client");
    }
}


void client::connect_to(const char *hostAddress, const in_port_t port) {
    struct sockaddr_in server{};
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;

    server.sin_port = port;

    if (inet_aton(hostAddress, &server.sin_addr) == 0) {
        my_error("Internet host address is invalid");
        throw runtime_error("Cannot parse host");

    }

    if (connect(socket_fd, reinterpret_cast<sockaddr *>(&server), sizeof(sockaddr_in)) == -1) {
        my_error("Cannot connect");
        throw runtime_error("Cannot connect");
    }
}

string client::request(const string& message) {
    if (send(socket_fd, message.data(), message.length() + 1, 0) != message.length() + 1) { //+1 for null terminal
        my_error("Cannot send message");
    }
    memset(buffer, 0, BUFFER_SIZE);
    ssize_t readed = read(socket_fd, buffer, BUFFER_SIZE);
    if (readed == -1) {
        my_error("Cannot receive response");
    }
    return string(buffer);
}
