#include <iostream>
#include <unistd.h>
#include <cstring>
#include <netinet/in.h>

const int PROTOCOL_FAMILY = AF_INET;
const int ATTEMPT_NUMBER = 5;
const int BUFFER_SIZE = 1024;
const int PORT = 8080;

void printErr(const std::string& message) {
    fprintf(stderr, "ERROR %s: %s\n", message.c_str(), strerror(errno));
}

void printUsage() {
    printf("Usage: <port>\n"
           "Default port: 8080\n\n");
}

void printGreetings() {
    printf("Echo server is started.\n\n");
}

int openSocket() {
    int openedSocket = socket(PROTOCOL_FAMILY, SOCK_STREAM, 0);

    if (openedSocket < 0) {
        printErr("Unable to open socket");
        exit(EXIT_FAILURE);
    }

    return openedSocket;
}

void bind(int listener, int port) {
    sockaddr_in serverAddress{};

    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_family = PROTOCOL_FAMILY;
    serverAddress.sin_port = htons(port);

    if (bind(listener, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(sockaddr_in)) < 0) {
        printErr("Unable to bind");
        close(listener);
        exit(EXIT_FAILURE);
    }
}

void listen(int listener) {
    if (listen(listener, ATTEMPT_NUMBER) < 0) {
        printErr("Unable to listen");
        close(listener);
        exit(EXIT_FAILURE);
    }
}

int accept(int socket) {
    sockaddr_in client{};
    socklen_t clientSize;

    int accepted = accept(socket, reinterpret_cast<sockaddr*>(&client), &clientSize);
    if (accepted < 0) {
        printErr("Unable to accept");
        close(accepted);
        exit(EXIT_FAILURE);
    }

    return accepted;
}

void echo(int listener, int port) {
    bind(listener, port);

    listen(listener);

    for (int i = 0; i < ATTEMPT_NUMBER; ++i) {
        int acceptedFd = accept(listener);

        char buffer[BUFFER_SIZE] = {};

        int readed = read(acceptedFd, buffer, BUFFER_SIZE);
        if (readed < 0) {
            printErr("Unable to read");
            close(acceptedFd);
            exit(EXIT_FAILURE);
        }

        if (send(acceptedFd, buffer, readed, 0) != readed) {
            printErr("Unable to send");
        }

        close(acceptedFd);

    }

}

int main(int argc, char** argv) {
    printGreetings();
    printUsage();
    int port = PORT;

    if (argc == 2) {
        port = atoi(argv[1]);
    }

    int listener = openSocket();

    echo(listener, port);

    close(listener);
    exit(EXIT_SUCCESS);
}