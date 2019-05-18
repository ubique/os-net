//
// Created by Anton Shelepov on 2019-05-17.
//

#include "server.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>
#include <vector>
#include <sys/types.h>
#include "../socket_descriptor/socket_descriptor.h"

server_exception::server_exception(std::string const& msg) : std::runtime_error(msg + ": " + strerror(errno)) {}

void server::log(const std::string& msg) {
    std::cout << "server: " + msg << std::endl;
}

server::server(std::string const& address, int port) : socket_fd() {

    if (!socket_fd.valid()) {
        throw server_exception("Couldn't create socket");
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(address.data());
    addr.sin_port = htons(port);

    if (bind(*socket_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1) {
        throw server_exception("Couldn't bind server address to the socket");
    }

    log("Server was deployed successfully");
}

void server::run() {
    if (listen(*socket_fd, BACKLOG_QUEUE_SIZE) == -1) {
        throw server_exception("Couldn't start listening to the socket");
    }

    log("Started to listen to the socket");

    while (true) {
        log("Waiting for connection");
        sockaddr_in client_addr = {0};

        socklen_t client_addr_len = sizeof(client_addr);
        socket_descriptor client_socket(accept(*socket_fd, reinterpret_cast<struct sockaddr*>(&client_addr), &client_addr_len));

        if (!client_socket.valid()) {
            log("Couldn't accept socket from client");
            continue;
        }

        handle_connection(client_socket, client_addr);
    }
}

void server::handle_connection(socket_descriptor const& client_socket, sockaddr_in const& client_addr) {
    log("Accepted socket from client with address " + std::string(inet_ntoa(client_addr.sin_addr)));
    while (true) {
        std::vector<char> buffer(BUFFER_SIZE);

        ssize_t message_len = recv(*client_socket, buffer.data(), BUFFER_SIZE, 0);
        if(message_len == -1) {
            //log("Failed to receive message");
            continue;
        }

        if(message_len == 0) {
            log("Client disconnected");
            return;
        }

        if(message_len > 0) {
            respond(client_socket, buffer, message_len);
        }
    }

}

void server::respond(socket_descriptor const& client_socket, std::vector<char>& buffer, ssize_t message_len) {
    log("Received message successfully");

    ssize_t was_sent = 0;
    size_t tries_to_send = TRIES_NUMBER;
    while (was_sent < message_len && tries_to_send) {
        ssize_t cnt = send(*client_socket, buffer.data() + was_sent, static_cast<size_t>(message_len - was_sent), 0);
        if (cnt == -1) {
            tries_to_send--;
            continue;
        }
        was_sent += cnt;
    }

    log(was_sent == message_len ? "Responded successfully" : "Failed to respond");
}

int main(int argc, char* argv[]) {
    std::string address = "127.0.0.1";
    int port = 90190;
    if (argc < 3) {
        std::cout << "Expected address and port number; default values will be used: 127.0.0.1 90190" << std::endl;
    } else {
        address = argv[1];
        port = std::stoi(argv[2]);
    }

    try {
        server serv(address, port);
        serv.run();
    } catch (server_exception& e) {
        std::cout << e.what() << std::endl;
    }
}