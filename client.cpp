#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

const int BUFF_SIZE = 1024;
const int PORT = 8080;
const char* IP = "127.0.0.1";


void show_err(const std::string& error) {
    fprintf(stderr, "ERROR: %s\n", error.c_str());
}

void show_usage() {
    printf("Usage: [port [ip [message]]]\n"
           "by default values are\n"
           "\tPort: 8080\n"
           "\tAddress: 127.0.0.1\n"
           "\tMessage: Hello, World!\n\n");
}


int open_socket() {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd < 0) {
        show_err("Cannot open socket");
        exit(EXIT_FAILURE);
    }

    return socket_fd;
}


void start(int socket, int port, int address, std::string &message) {
    sockaddr_in serverAddress{};

    serverAddress.sin_addr.s_addr = address;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_family = AF_INET;

    if (connect(socket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(sockaddr_in)) < 0) {
        show_err("Cannot connect client");
        exit(EXIT_FAILURE);
    }

    const char* messageData = message.c_str();

    size_t messageSize = strlen(messageData);
    if (send(socket, messageData, messageSize, 0) != messageSize) {
        show_err("Cannot send message");
        exit(EXIT_FAILURE);
    }

    char reply[BUFF_SIZE] = {};
    if (read(socket, reply, BUFF_SIZE) < 0) {
        show_err("Cannot read");
        exit(EXIT_FAILURE);
    }
    printf("Server reply");

    printf("Server reply: %s\n", reply);
}

int main(int argc, char** argv) {    
    if (argc > 2) {
        show_usage();
    }

    int port = PORT;

    int address = inet_addr(IP);

    std::string request = "Hello, World!";

    switch (argc) {
        case 4:
            request = argv[3];
        case 3: 
            address = inet_addr(argv[2]);
        case 2: 
            port = atoi(argv[1]);
            break;
        default:
            break;
    }
    
    int socket = open_socket();

    start(socket, port, address, request);

    close(socket);

    exit(EXIT_SUCCESS);
}
