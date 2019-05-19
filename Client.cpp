//
// Created by Павел Пономарев on 2019-05-19.
//

#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

void printHelpMessage() {
    std::cout << "client <address> -- run client; \n "
                 "client help | -help | --help -- print this message.\n";
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
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        return -1;
    }
    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        return -2;
    }
    return sock;
}

bool connectSocket(int sock, int address) {
    sockaddr_in addr;
    addr.sin_port = htons(address);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_family = AF_INET;
    return (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) >= 0);
}

int main(int argc, char **argv) {
    char serverResponse[1024];
    if (argc < 2) {
        printErrorMessage("incorrect number of arguments", false);
        printHelpMessage();
        return EXIT_FAILURE;
    }
    std::string argument(argv[1]);
    int address = parseInt(argument);
    int sock = openSocket();
    connectSocket(sock, address);

    std::string request;
    std::cout << "Request: ";
    getline(std::cin, request);

    send(sock, request.data(), request.length(), 0);

    if (recv(sock, serverResponse, 1024, 0) < 0) {
        printErrorMessage("can't receive message from socket");
    }

    std::cout << "Server response: " << serverResponse << std::endl;

    close(sock);
    return EXIT_SUCCESS;
}