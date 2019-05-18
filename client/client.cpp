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
    log("Request server, message: " + message);

    std::vector<char> buffer(BUFFER_SIZE);
    std::string respond;

    size_t tries_to_send = TRIES_NUMBER;
    while (tries_to_send) {
        ssize_t was_sent = send(*socket_fd, message.data(), message.size(), 0);
        if (was_sent == -1) {
            tries_to_send--;
            log("Failed to send message" + std::string(tries_to_send ? "trying again..." : ""));
            continue;
        }

        log("Request was sent successfully");

        ssize_t cnt = recv(*socket_fd, buffer.data(), static_cast<size_t>(was_sent), 0);
        if (cnt == -1) {
            log("Couldn't receive respond");
        } else {
            respond = std::string(buffer.data());
            std::cout << "Respond: " + respond << std::endl;
        }
        break;

    }

    if (tries_to_send == 0) {
        std::cout << "Request was declined" << std::endl;
    }

    return respond;
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

        request(req);
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