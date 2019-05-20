//
// Created by roman on 20.05.19.
//

#include <memory.h>
#include <arpa/inet.h>
#include "client.h"
#include "error.h"
#include <unistd.h>
#include <stdexcept>
#include <sys/types.h>
#include <sys/socket.h>

using std::runtime_error;
using std::string;

client::client() {
    tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket == -1) {
        error("Socket cannot be created");
    }
}


void client::connect_to(const char *hostAddress, const in_port_t port) {
    struct sockaddr_in server{};
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;

    server.sin_port = port;

    if (inet_aton(hostAddress, &server.sin_addr) == 0) {
        error("Internet host address is invalid");
    }

    if (connect(tcp_socket, reinterpret_cast<sockaddr *>(&server), sizeof(sockaddr_in)) == -1) {
        error("Cannot connect");
    }
}

string client::request(const string& message) {
    if (send(tcp_socket, message.data(), message.length() + 1, 0) != message.length() + 1) { //+1 for null terminal
        error("Cannot send message");
    }
    memset(buffer, 0, BUFFER_SIZE);
    ssize_t readed = read(tcp_socket, buffer, BUFFER_SIZE);
    if (readed == -1) {
        error("Cannot receive response");
    }
    return string(buffer);
}

void client::disconnect() {
    if (close(tcp_socket) == 1)
        error("Cannot close socket");
}
