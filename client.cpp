#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

const int PROTOCOL_FAMILY = AF_INET;
const int ATTEMPT_NUMBER = 5;
const int BUFFER_SIZE = 1024;
const int PORT = 8080;
const char* IP = "127.0.0.1";


void printErr(const std::string& message) {
    fprintf(stderr, "ERROR %s: %s\n", message.c_str(), strerror(errno));
}

void printUsage() {
    printf("Usage: <port> <ip> <request>\n"
           "Client is connected to default\n"
           "Default values:\n"
           "   Port: 8080\n"
           "   Address: 127.0.0.1\n"
           "   Request: request\n\n");
}

void printGreetings() {
    printf("Echo client greets you!\n\n");
}

int openSocket() {
    int openedSocket = socket(PROTOCOL_FAMILY, SOCK_STREAM, 0);

    if (openedSocket < 0) {
        printErr("Unable to open socket");
        close(openedSocket);
        exit(EXIT_FAILURE);
    }

    return openedSocket;
}

void connect(int socket, int port, int address) {
    sockaddr_in serverAddress{};

    serverAddress.sin_addr.s_addr = address;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_family = PROTOCOL_FAMILY;

    if (connect(socket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(sockaddr_in)) < 0) {
        printErr("Unable to connect client");
        close(socket);
        exit(EXIT_FAILURE);
    }
}

bool checkSend(int socket, const char* msg) {
    ssize_t sent = 0;
    size_t msgSize = strlen(msg);
    ssize_t rest = msgSize;

    while (sent != msgSize) {
        ssize_t del = send(socket, msg + sent, rest, 0);

        if (del < 0) {
            return false;
        } else if (del == 0) {
            break;
        } else {
            sent += del;
            rest -= del;
        }
    }

    return true;
}

bool checkReply(int socket, const char* msg, char reply[]) {
    ssize_t received = 0;
    ssize_t msgSize = strlen(msg);

    while (received != msgSize) {
        ssize_t del = read(socket, reply, BUFFER_SIZE);

        if (del < 0) {
            return false;
        } else if (del == 0) {
            break;
        } else {
            for (int i = 0; i < del; ++i) {
                printf("%c", reply[i]);
            }
            printf("\n");

            received += del;
        }
    }

    return true;
}

void respond(int socket, std::string& msg) {
    const char* msgData = msg.c_str();

    if (!checkSend(socket, msgData)) {
        printErr("Unable to send message");
        close(socket);
        exit(EXIT_FAILURE);
    }

    char reply[BUFFER_SIZE] = {};

    printf("Server reply:\n");

    if (!checkReply(socket, msgData, reply)) {
        printErr("Unable to read");
        close(socket);
        exit(EXIT_FAILURE);
    }


}

int main(int argc, char** argv) {
    printGreetings();

    int port = PORT;
    int address = inet_addr(IP);
    std::string request = "request";

    switch (argc) {
        case 4: {
            request = argv[3];
        }
        case 3: {
            address = inet_addr(argv[2]);
        }
        case 2: {
            port = atoi(argv[1]);
            break;
        }
        default:
            printUsage();
    }

    int socket = openSocket();
    connect(socket, port, address);
    respond(socket, request);

    if (close(socket) < 0) {
        printErr("Unable to close socket");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}