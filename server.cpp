//
// Created by SP4RK on 18/05/2019.
//
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <netinet/in.h>
#include <cstring>

const int BUFFER_SIZE = 1024;

int bind(int port) {
    int listenerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenerSocket < 0) {
        std::cerr << "Error while opening socket " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    int option = 1;
    int status;
    status = setsockopt(listenerSocket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    if (status == -1) {
        std::cerr << "Error while setting option to socket " << strerror(errno) << std::endl;
    }
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    status = bind(listenerSocket, (struct sockaddr *) &address, sizeof(address));
    if (status == -1) {
        std::cerr << "Error while binding to socket " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    return listenerSocket;
}

void closeSocket(int socket) {
    if (close(socket) == -1) {
        std::cerr << "Error while closing socket" << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "You are expected to enter 2 arguments: <port> <requests to accept>" << std::endl;
        exit(EXIT_FAILURE);
    }
    int listenerSocket = bind(atoi(argv[1]));
    listen(listenerSocket, 1);
    for (int i = 0; i < atoi(argv[2]); ++i) {
        int connectionSocket = accept(listenerSocket, nullptr, nullptr);
        if (connectionSocket == -1) {
            std::cerr << "Error while establishing connection with client: " << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }
        char buffer[BUFFER_SIZE];

        while (ssize_t dataReceived = recv(connectionSocket, buffer, BUFFER_SIZE, 0)) {

            if (dataReceived < 0) {
                std::cerr << "Error while receiving message" << strerror(errno) << std::endl;
                closeSocket(connectionSocket);
                exit(EXIT_FAILURE);
            } else if (dataReceived == 0) {
                break;
            } else {
                ssize_t dataSent = 0;
                while (dataSent < dataReceived) {
                    ssize_t curSent = send(connectionSocket, buffer + dataSent, dataReceived - dataSent, 0);
                    if (curSent < 0) {
                        std::cerr << "Error while sending message" << strerror(errno) << std::endl;
                        closeSocket(connectionSocket);
                        exit(EXIT_FAILURE);
                    } else {
                        dataSent += curSent;
                    }
                }
            }
        }
    }
    close(listenerSocket);
}