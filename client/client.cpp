//
// Created by Anton Shelepov on 2019-05-17.
//

#include "client.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <iostream>
#include <string>
#include <vector>
#include "../utils/utils.h"

client_exception::client_exception(std::string const& msg) : std::runtime_error(msg + ": " + strerror(errno)) {}

client::client() : socket_fd() {
    if (!socket_fd.valid()) {
        throw client_exception("Couldn't create socket");
    }

    log("Client was deployed successfully");
}

void client::establish_connection(std::string const& address, int port) {
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(address.data());

    if (connect(*socket_fd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(sockaddr_in)) == -1) {
        throw client_exception("Couldn't connect to the server " + address);
    }

    log("Connected to server " + address + ", port " + std::to_string(port));
}

std::string client::request(std::string const& message) {
    return read(*socket_fd, send(*socket_fd, message));
}

std::string client::read(int desc, size_t expected) {
    std::string message = utils::read(desc, expected);
    if (message.size() != expected) {
        throw client_exception("Failed to receive full message");
    }

    log("Read message successfully: " + message);
    return message;
}

size_t client::send(int desc, std::string const& message) {
    log("Sending message " + message + "...");
    size_t was_sent = utils::send(desc, message);

    if (was_sent == 0) {
        throw client_exception("Failed to send request");
    }
    if (was_sent != message.size()) {
        log("Not full message was sent");
    } else {
        log("Send message successfully");
    }

    return was_sent;
}

void client::log(std::string const& msg) {
    std::cout << "client: " << msg << std::endl;
}

void client::run() {
    while (true) {
        std::string req;
        std::cout << "Enter your request: ";
        std::cout.flush();

        getline(std::cin, req);

        if (req == "exit") {
            break;
        }

        try {
            request(req);
        } catch (client_exception& e) {
            log(e.what());
        }
    }
}

int main(int argc, char* argv[]) {
    std::cout << "Simple echo client. Server will echo your requests. Type \"exit\" request to exit";
    std::string address = "127.0.0.1";
    int port = 90190;
    if (argc < 3) {
        std::cout << "Expected address and port number; default values will be used: 127.0.0.1 90190" << std::endl;
    } else {
        address = argv[1];
        port = std::stoi(argv[2]);
    }

    try {
        client cli;
        cli.establish_connection(address, port);
        cli.run();
    } catch (client_exception& e) {
        std::cout << e.what() << std::endl;
    }
}