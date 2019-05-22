//
// Created by Noname Untitled on 19.05.19.
//

#include <iostream>
#include <sstream>
#include <vector>
#include <map>

#include <unistd.h>
#include <string.h>

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

Server::Server(const char *port) : mServerSocket(-1), mSocketAddress(), mDatabase() {
    std::cout << serverCommands << std::endl;

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

    memset(&mSocketAddress, 0, sizeof(sockaddr_in));
    mSocketAddress.sin_family = AF_INET;
    mSocketAddress.sin_port = htons(mPort);
    mSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
}

Server::~Server() {
    if (mServerSocket != -1) {
        shutdown(mServerSocket, SHUT_RDWR);
        close(mServerSocket);
    }

    std::cout << "Terminate server!" << std::endl;
}

void Server::createBinding() {
    mServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mServerSocket == -1) {
        perror("Socket creation");
        exit(EXIT_FAILURE);
    }

    int bindResult = bind(mServerSocket, (struct sockaddr *) &mSocketAddress, sizeof(mSocketAddress));
    if (bindResult == -1) {
        perror("Binding creation");
        close(mServerSocket);
        exit(EXIT_FAILURE);
    }

    if (bindResult == 0) {
        std::cout << "Binding successful!" << std::endl;
    }
}

void Server::run() {
    listen(mServerSocket, SOMAXCONN);

    std::cout << "Waiting for incoming socket..." << std::endl;

    while (true) {
        int receivedSocket = accept(mServerSocket, nullptr, nullptr);
        if (receivedSocket == -1) {
            continue;
        }

        std::cout << "Process incoming socket..." << std::endl;

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

        shutdown(receivedSocket, SHUT_RDWR);
        close(receivedSocket);

        std::cout << "Waiting for incoming socket..." << std::endl;
        sleep(1);
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