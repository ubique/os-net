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
#include "../utils/utils.h"

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
    alive = true;
    while (alive) {
        try {
            send(*client_socket, read(*client_socket));
        } catch (server_exception& e) {
            std::cout << e.what() << std::endl;
        }
    }

}

std::string server::read(int desc) {
    std::string message = utils::read(desc);
    if (message.length() == 0) {
        alive = false;
        throw server_exception("Client disconnected");
    }
    log("Read message successfully: " + message);
    return message;
}

void server::send(int desc, std::string const& message) {
    log("Sending message " + message + "...");
    size_t was_sent = utils::send(desc, message);

    if (was_sent == 0) {
        alive = false;
        throw server_exception("Client disconnected");
    }
    log("Send message successfully");
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