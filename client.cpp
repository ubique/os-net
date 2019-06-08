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

int connect(int port) {
    int openedSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (openedSocket < 0) {
        std::cerr << "Error while opening socket " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    int option = 1;
    int status;
    status = setsockopt(openedSocket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    if (status == -1) {
        std::cerr << "Error while setting option to socket " << strerror(errno) << std::endl;
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    status = connect(openedSocket, (struct sockaddr*) &address, sizeof(address));
    if (status == -1) {
        std::cerr << "Error while connecting to socket " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    return openedSocket;
}

void closeSocket(int socket) {
	if (close(socket) == -1) {
		std::cerr << "Error while closing socket" << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
	}
}

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "You are expected to enter 2 arguments: <port> <message>" << std::endl;
        exit(EXIT_FAILURE);
    }
    int connectedSocket = connect(atoi(argv[1]));
    const char* requestMessage = argv[2];
	
    char replyBuffer[BUFFER_SIZE];
	ssize_t dataSent = 0;
    size_t requestMessageSize = strlen(requestMessage);
    ssize_t leftDataToSend = requestMessageSize;

    while (dataSent < requestMessageSize) {
        ssize_t curSent = send(connectedSocket, requestMessage + dataSent, leftDataToSend, 0);
        if (curSent < 0) {
            std::cerr << "Error while sending message" << strerror(errno) << std::endl;
			closeSocket(connectedSocket);
			exit(EXIT_FAILURE);
        } else if (curSent == 0) {
            break;
        } else {
            dataSent += curSent;
            leftDataToSend -= curSent;
        }
    }
	
    std::cout << "Reply:" << std::endl;
	ssize_t receivedData = 0;
    while (receivedData < requestMessageSize) {
        ssize_t curReceived = read(connectedSocket, replyBuffer, BUFFER_SIZE);

        if (curReceived < 0) {
            std::cerr << "Error while reading receiving message from socket" << strerror(errno) << std::endl;
			closeSocket(connectedSocket);
			exit(EXIT_FAILURE);
        } else if (curReceived == 0) {
            break;
        } else {
            for (int i = 0; i < curReceived; ++i) {
                std::cout << replyBuffer[i];
            }
            receivedData += curReceived;
            std::cout << std::endl;
        }
    }
	closeSocket(connectedSocket);
}