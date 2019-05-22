//
// Created by Noname Untitled on 20.05.19.
//

#include <iostream>

#include <unistd.h>
#include <string.h>
#include <netdb.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "Client.h"

#define MIN_PORT    2'001
#define MAX_PORT    65'534
#define MESSAGE_BUFFER_SIZE 1024

Client::Client(const char *addr, const char *port, std::string msg) : mClientSocket(-1), mSocketAddress(),
                                                                      response(nullptr), message(std::move(msg)) {

    try {
        int numericPort = std::stoi(port);
        if (numericPort < MIN_PORT || numericPort > MAX_PORT) {
            std::cerr << "Invalid port. Port must be decimal value between "
                      << MIN_PORT << " and " << MAX_PORT << "." << std::endl;

            exit(EXIT_FAILURE);
        }

        mPort = numericPort;
    } catch (std::invalid_argument &e) {
        std::cerr << "Invalid port." << std::endl;
        exit(EXIT_FAILURE);
    }

    memset(&mSocketAddress, 0, sizeof(mSocketAddress));
    mSocketAddress.sin_family = AF_INET;
    mSocketAddress.sin_port = htons(mPort);
    int addrConvertResult = inet_pton(AF_INET, addr, &(mSocketAddress.sin_addr));
    if (addrConvertResult != 1) {
        std::cerr << "Invalid address." << std::endl;
        exit(EXIT_FAILURE);
    }
}

Client::~Client() {
    if (mClientSocket != -1) {
        shutdown(mClientSocket, SHUT_RDWR);
        close(mClientSocket);
    }
}

void Client::createConnection() {
    mClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    int connectResult = connect(mClientSocket, (struct sockaddr *) &mSocketAddress, sizeof(mSocketAddress));
    if (connectResult == -1) {
        perror("Connection creation");
        shutdown(mClientSocket, SHUT_RDWR);
        close(mClientSocket);
        exit(EXIT_FAILURE);
    }

    if (connectResult == 0) {
        std::cout << "Connection created!" << std::endl;
    }
}

void Client::sendInfo() {
    int sendResult = send(mClientSocket, message.c_str(), message.length() + 1, 0);
    if (sendResult == -1) {
        perror("Send message error");
        exit(EXIT_FAILURE);
    }

    response = new char[MESSAGE_BUFFER_SIZE];
    memset(response, 0, MESSAGE_BUFFER_SIZE);

    int receiveResult = recv(mClientSocket, response, MESSAGE_BUFFER_SIZE, 0);
    if (receiveResult == -1) {
        perror("Receiving response error");
        exit(EXIT_FAILURE);
    }
}

char *Client::getResponse() const {
    return response;
}