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
           "Server is running on default\n"
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

bool checkSend(int socket, ssize_t received, char reply[]) {
    ssize_t sent = 0;
    size_t rest = received;

    while (sent != received) {
        ssize_t del = send(socket, reply + sent, rest, 0);

        if (del < 0) {
            return false;
        } else {
            sent += del;
            rest -= del;
        }
    }

    return true;
}


void echo(int listener, int port) {
    bind(listener, port);

    listen(listener);

    for (int i = 0; i < ATTEMPT_NUMBER; ++i) {
        int acceptedFd = accept(listener);

        char buffer[BUFFER_SIZE] = {};

        while (ssize_t received = read(acceptedFd, buffer, BUFFER_SIZE)) {

            if (received < 0) {
                printErr("Unable to receive");
                break;
            } else if (received == 0) {
                break;
            } else {
                if (!checkSend(acceptedFd, received, buffer)) {
                    printErr("Unable to send reply");
                    break;
                }
            }
        }

        if (close(acceptedFd) < 0) {
            printErr("Unable to close accepted");
            exit(EXIT_FAILURE);
        }

    }

}

int main(int argc, char** argv) {
    printGreetings();
    int port = PORT;

    if (argc == 2) {
        port = atoi(argv[1]);
    } else {
        printUsage();
    }

    int listener = openSocket();

    echo(listener, port);

    if (close(listener) < 0) {
        printErr("Unable to close listener");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}