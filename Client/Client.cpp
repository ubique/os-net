//
// Created by Noname Untitled on 20.05.19.
//
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <map>

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "Client.h"

#define MIN_PORT    2'001
#define MAX_PORT    65'534
#define MESSAGE_BUFFER_SIZE 1024

Client::Client(const char *address, const char *port, const std::string &message) : mAddress(address),
                                                                                    mPort(port), serverHint(),
                                                                                    result(nullptr),
                                                                                    serverAddrInfo(nullptr),
                                                                                    message(message) {
    try {
        int numericPort = std::stoi(mPort);
        if (numericPort < MIN_PORT || numericPort > MAX_PORT) {
            std::cerr << "Invalid port. Port must be decimal value between "
                      << MIN_PORT << " and " << MAX_PORT << "." << std::endl;

            exit(EXIT_FAILURE);
        }
    } catch (std::invalid_argument &e) {
        std::cerr << "Invalid port." << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Client::createConnection() {
    findSuitableAddrs();

    serverSocketDescr = -1;
    serverAddrInfo = result;
    for (; serverAddrInfo != nullptr; serverAddrInfo = serverAddrInfo->ai_next) {
        serverSocketDescr = socket(serverAddrInfo->ai_family, serverAddrInfo->ai_socktype, serverAddrInfo->ai_protocol);
        if (serverSocketDescr == -1) {
            continue;
        }

        int connectionResult = connect(serverSocketDescr, serverAddrInfo->ai_addr, serverAddrInfo->ai_addrlen);

        if (connectionResult == 0) {
            std::cout << "Connected!" << std::endl;
            break;
        }

        close(serverSocketDescr);
    }

    if (serverAddrInfo == nullptr) {
        std::cout << "Connection failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);
}

void Client::run() {
    int sendResult = send(serverSocketDescr, message.c_str(), message.length() + 1, 0);
    if (sendResult == -1) {
        perror("Send message error");
        exit(EXIT_FAILURE);
    }

    response = new char[MESSAGE_BUFFER_SIZE];
    memset(response, 0, MESSAGE_BUFFER_SIZE);

    int receiveResult = recv(serverSocketDescr, response, MESSAGE_BUFFER_SIZE, 0);
    if (receiveResult == -1) {
        perror("Receiving response error");
        exit(EXIT_FAILURE);
    }

}

char *Client::getResponse() const {
    return response;
}

void Client::findSuitableAddrs() {
    memset(&serverHint, 0, sizeof(addrinfo));
    serverHint.ai_family = AF_UNSPEC;
    serverHint.ai_socktype = SOCK_STREAM;
    serverHint.ai_flags = 0;
    serverHint.ai_protocol = 0;

    serverHint.ai_addrlen = 0;
    serverHint.ai_addr = nullptr;
    serverHint.ai_next = nullptr;
    serverHint.ai_canonname = nullptr;

    int getAddrResult = getaddrinfo(mAddress, mPort, &serverHint, &result);

    if (getAddrResult != 0) {
        std::cerr << gai_strerror(getAddrResult) << std::endl;
        exit(EXIT_FAILURE);
    }
}