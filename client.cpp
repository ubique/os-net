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
    do_request(message);
    if (message == "stop")
        return "";
    memset(buffer, 0, BUFFER_SIZE);
    receive_response();
    return string(buffer);
}

void client::receive_response() {
    ssize_t count = 0;
    {
        ssize_t received = 0;
        while (received != sizeof(ssize_t)) {
            ssize_t tmp = read(socket_fd, (char*)(&count) + received, sizeof(ssize_t) - received);
            if (tmp == -1 || tmp == 0) {
                my_error("Cannot response");
                return;
            }
            received += tmp;
        }
    }
    {
        ssize_t received = 0;
        while (received != count) {
            ssize_t tmp = read(socket_fd, buffer + received, count - received);
            if (tmp == -1 || tmp == 0) {
                my_error("Cannot response");
                return;
            }
            received += tmp;
        }
    }
}

void client::do_request(const string &msg){
    ssize_t count = msg.length() + 1;
    {
        ssize_t sent = 0;
        while (sent != sizeof(ssize_t)) {
            ssize_t written = write(socket_fd, (char*)(&count) + sent, sizeof(ssize_t) - sent);
            if (written == -1) {
                my_error("Cannot do request");
                return;
            }
            sent += written;
        }
    }
    {
        ssize_t sent = 0;
        while (sent != count) {
            ssize_t written = write(socket_fd, msg.data() + sent, count - sent);
            if (written == -1) {
                my_error("Cannot response");
                return;
            }
            sent += written;
        }
    }
    
//    if (send(socket_fd, .data(), message.length() + 1, 0) != message.length() + 1) { //+1 for null terminal
//        my_error("Cannot send message");
//    }
}
