//
// Created by Павел Пономарев on 2019-05-19.
//

#include "Server.h"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

int Server::openSocket() {
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
        return -1;
    }
    int opt = 1;
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        return -2;
    }
    return listener;
}

bool Server::bindSocket(int listener, int address) {
    sockaddr_in addr{};
    addr.sin_port = htons(address);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    return bind(listener, (struct sockaddr *) &addr, sizeof(addr)) >= 0;
}

void Server::execute(int socket) {
    char buf[1024];
    ssize_t bytes = recv(socket, buf, 1024, 0);
    buf[bytes] = '\0';
    send(socket, buf, bytes, 0);
    close(socket);
}

bool Server::run(std::string const& argument) {
    int address = parseInt(argument);

    int listener = openSocket();
    if (listener < 0) {
        if (listener == -1) {
            printErrorMessage("can't create an endpoint for communication");
            return false;
        } else {
            printErrorMessage("can't get/set options on socket");
        }
    }

    if (!bindSocket(listener, address)) {
        printErrorMessage("can't bind socket");
        return false;
    }

    listen(listener, 1);

    for (size_t i = 0; i < 7; ++i) {
        int sock = accept(listener, nullptr, nullptr);
        if (sock < 0) {
            printErrorMessage("can't accept connection on a socket");
            return false;
        }
        execute(sock);
    }

    close(listener);
    return true;
}

void printHelpMessage() {
    std::cout << "os-net-server <address> -- run server; \n "
                 "os-net-server help | -help | --help -- print this message.\n";
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printErrorMessage("incorrect number of arguments", false);
        printHelpMessage();
        return EXIT_FAILURE;
    }
    std::string argument(argv[1]);

    if (argument == "help" || argument == "-help" || argument == "--help") {
        printHelpMessage();
        return EXIT_SUCCESS;
    }
    Server server;

    return server.run(argument) ? EXIT_SUCCESS : EXIT_FAILURE;
}
