//
// Created by Павел Пономарев on 2019-05-19.
//

#include "Client.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

bool Client::run(std::string const& argument) {
    char serverResponse[BUFFER_SIZE];
    int address = parseInt(argument);
    int sock = openSocket();
    connectSocket(sock, address);

    std::string request;
    std::cout << "Request: ";
    getline(std::cin, request);

    ssize_t sent = 0;
    ssize_t left = request.size();

    while (sent < request.size()) {
        ssize_t currentMessage = send(sock, request.data() + sent, left, 0);
        if (currentMessage == -1) {
            printErrorMessage("sending failure");
            if (close(sock) == -1) {
                printErrorMessage("Fail to close");
            }
            exit(EXIT_FAILURE);
        } else {
            sent += currentMessage;
            left -= currentMessage;
        }
    }
    ssize_t received = 0;

    std::cout << "Server response:" << std::endl;
    while (received < request.size()) {
        ssize_t res = recv(sock, serverResponse, BUFFER_SIZE, 0);
        if (res == 0) {
            break;
        }
        if (res == -1) {
            printErrorMessage("Receiving failure");
            if (close(sock) == -1) {
                printErrorMessage("Fail to close");
            }
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < res; ++i) {
            std::cout << serverResponse[i];
        }
        received += res;
    }

    std::cout << std::endl;

    if (close(sock) == -1) {
        printErrorMessage("Fail to close");
    }

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
    std::cout << "os-net-client <address> -- run client; \n "
                 "os-net-client help | -help | --help -- print this message.\n";
}


int main(int argc, char** argv) {
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
