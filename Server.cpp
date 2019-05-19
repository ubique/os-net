//
// Created by Павел Пономарев on 2019-05-19.
//

#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

void printHelpMessage() {
    std::cout << "server <address> <requestsNum> -- run server (requestsNum is optional); \n "
                 "server help | -help | --help -- print this message.\n";
}

void printErrorMessage(std::string const& message, bool error = true) {
    std::cout << "Error occurred: " << message << ". ";
    if (error) {
        std::cout << std::strerror(errno);
    }
    std::cout << std::endl;
}

int parseInt(std::string const& str) {
    int value;
    std::istringstream stream(str);
    stream >> value;
    return value;
}

int openSocket() {
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

bool bindSocket(int listener, int address) {
    sockaddr_in addr{};
    addr.sin_port = htons(address);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    return bind(listener, (struct sockaddr *) &addr, sizeof(addr)) >= 0;
}

void execute(int sock) {
    char buf[1024];
    ssize_t bytes = recv(sock, buf, 1024, 0);
    buf[bytes] = '\0';
    send(sock, buf, bytes, 0);
    close(sock);
}

int main(int argc, char** argv) {
    int requestsNumber = 10;
    if (argc < 2) {
        printErrorMessage("incorrect number of arguments", false);
        printHelpMessage();
        return EXIT_FAILURE;
    }
    std::string argument(argv[1]);

    if (argument == "help" || argument == "-help" || argument == "--help") {
        printHelpMessage();
        return EXIT_SUCCESS;
    }

    int address = parseInt(argument);
    if (argc > 2) {
        std::string request(argv[2]);
        requestsNumber = parseInt(request);
    }

    int listener = openSocket();
    if (listener < 0) {
        if (listener == -1) {
            printErrorMessage("can't create an endpoint for communication");
            return EXIT_FAILURE;
        } else {
            printErrorMessage("can't get/set options on socket");
        }
    }

    if (!bindSocket(listener, address)) {
        printErrorMessage("can't bind socket");
        return EXIT_FAILURE;
    }

    listen(listener, 1);

    for (size_t i = 0; i < requestsNumber; ++i) {
        int sock = accept(listener, nullptr, nullptr);
        if (sock < 0) {
            printErrorMessage("can't accept connection on a socket");
            return EXIT_FAILURE;
        }
        execute(sock);
    }

    close(listener);
    return EXIT_SUCCESS;
}