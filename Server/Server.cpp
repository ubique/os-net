//
// Created by Noname Untitled on 19.05.19.
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

#include "../Person/Person.h"
#include "../Database/Database.h"

#include "Server.h"

#define MIN_PORT    2'001
#define MAX_PORT    65'534
#define MESSAGE_BUFFER_SIZE 1024

const std::string serverCommands = "INSERT <person_firstName> <person_lastName> <some_int_number> - Add \'number\' to \'person\'\n"
                                   "PRINT_ALL - Return all database info from the server\n"
                                   "PRINT_PERSON <person_firstName> <person_lastName> - Return info about person given from the server\n"
                                   "ECHO <some_message> - Echo the message given\n"
                                   "OPTIONS - Returns server options\n";

Server::Server(const char *mPort) : mPort(mPort), serverSocketDescr(-1),
                                    serverHint(), result(nullptr),
                                    serverAddrInfo(nullptr) {

    std::cout << serverCommands << std::endl;

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

void Server::createBinding() {
    findSuitableAddrs();

    serverSocketDescr = -1;
    serverAddrInfo = result;
    for (; serverAddrInfo != nullptr; serverAddrInfo = serverAddrInfo->ai_next) {

        serverSocketDescr = socket(serverAddrInfo->ai_family, serverAddrInfo->ai_socktype, serverAddrInfo->ai_protocol);
        if (serverSocketDescr == -1) {
            continue;
        }

        int bindResult = bind(serverSocketDescr, serverAddrInfo->ai_addr, serverAddrInfo->ai_addrlen);
        if (bindResult == 0) {
            std::cout << "Successful binding!" << std::endl;
            break;
        }

        int closeResult = close(serverSocketDescr);
        if (closeResult == -1) {
            perror("Socket close error.");
            exit(EXIT_FAILURE);
        }
    }

    if (serverAddrInfo == nullptr) {
        std::cerr << "Binding failed!" << std::endl;
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);
}

void Server::run() {
    listen(serverSocketDescr, 1);

    while (true) {
        int receivedSocket = accept(serverSocketDescr, nullptr, nullptr);
        if (receivedSocket == -1) {
            continue;
        }

        char message[MESSAGE_BUFFER_SIZE];
        memset(message, 0, MESSAGE_BUFFER_SIZE);

        int receiveResult = recv(receivedSocket, message, MESSAGE_BUFFER_SIZE, 0);
        if (receiveResult == -1) {
            perror("Receive message error");
            continue;
        }

        std::cout << "Client sent to server: " << message << std::endl;

        std::string response = processGivenCommand(message);

        int sendResult = send(receivedSocket, response.c_str(), response.length() + 1, 0);
        if (sendResult == -1) {
            perror("Send response error");
            continue;
        }

        close(receivedSocket);
        sleep(1);
    }
}

void Server::findSuitableAddrs() {
    memset(&serverHint, 0, sizeof(addrinfo));
    serverHint.ai_family = AF_UNSPEC;
    serverHint.ai_socktype = SOCK_STREAM;
    serverHint.ai_flags = AI_PASSIVE;
    serverHint.ai_protocol = 0;

    serverHint.ai_addrlen = 0;
    serverHint.ai_addr = nullptr;
    serverHint.ai_next = nullptr;
    serverHint.ai_canonname = nullptr;

    int getAddrResult = getaddrinfo(nullptr, mPort, &serverHint, &result);

    if (getAddrResult != 0) {
        std::cerr << gai_strerror(getAddrResult) << std::endl;
        exit(EXIT_FAILURE);
    }
}

std::string Server::processGivenCommand(const char *message) {
    std::string response;
    std::string errorReason;
    bool isCorrect = true;

    std::stringstream ss(message);

    std::string command;
    std::getline(ss, command, ' ');

    if (command == "INSERT") {
        if (ss.eof()) {
            errorReason = "Wrong INSERT arguments.";
            isCorrect = false;
        }

        std::string firstName, lastName, someNumber;
        std::getline(ss, firstName, ' ');
        std::getline(ss, lastName, ' ');
        std::getline(ss, someNumber);

        if (firstName.empty() || lastName.empty() || someNumber.empty()) {
            errorReason = "Wrong INSERT arguments.";
            isCorrect = false;
        }

        if (isCorrect) {
            mDatabase.insert(Person(firstName, lastName), someNumber);
            response = "Successful insertion!";
        }

    } else if (command == "PRINT_ALL") {
        response = mDatabase.printAll().str();
    } else if (command == "PRINT_PERSON") {
        if (ss.eof()) {
            errorReason = "Wrong PRINT_PERSON arguments.";
            isCorrect = false;
        }

        std::string firstName, lastName;
        std::getline(ss, firstName, ' ');
        std::getline(ss, lastName, ' ');

        if (firstName.empty() || lastName.empty()) {
            errorReason = "Wrong PRINT_PERSON arguments.";
            isCorrect = false;
        }

        if (isCorrect) {
            response = mDatabase.printPerson(Person(firstName, lastName)).str();
        }
    } else if (command == "ECHO") {
        std::getline(ss, response);
        if (response.empty()) {
            isCorrect = false;
        }
    } else if (command == "OPTIONS") {
        response = serverCommands;
    } else {
        errorReason = "Unknown command.";
        isCorrect = false;
    }

    if (!isCorrect) {
        return errorReason;
    }

    return response;
}