//
// Created by Павел Пономарев on 2019-05-19.
//

#include "Client.h"
#include <iostream>
#include <sstream>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

bool Client::run(std::string const& argument) {
    char serverResponse[1024];
    int address = parseInt(argument);
    int sock = openSocket();
    connectSocket(sock, address);

    std::string request;
    std::cout << "Request: ";
    getline(std::cin, request);

    send(sock, request.data(), request.length(), 0);

    if (recv(sock, serverResponse, BUFFER_SIZE, 0) < 0) {
        printErrorMessage("can't receive message from socket");
    }

    std::cout << "Server response: " << serverResponse << std::endl;

    close(sock);

    return true;
}

int Client::openSocket() {
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

bool Client::connectSocket(int socket, int address) {
    sockaddr_in addr{};
    addr.sin_port = htons(address);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_family = AF_INET;
    return (connect(socket, (struct sockaddr*) &addr, sizeof(addr)) >= 0);
}

void printHelpMessage() {
    std::cout << "client <address> -- run client; \n "
                 "client help | -help | --help -- print this message.\n";
}


int main(int argc, char **argv) {
    if (argc < 2) {
        printErrorMessage("incorrect number of arguments", false);
        printHelpMessage();
        return EXIT_FAILURE;
    }
    std::string argument(argv[1]);
    Client client;
    client.run(argument);
    return EXIT_SUCCESS;
}
