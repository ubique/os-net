#include <iostream>
#include <unistd.h>
#include <cstring>
#include <netinet/in.h>

const int ATTEMPT = 5;
const int BUFF_SIZE = 1024;
const int PORT = 8080;

void show_err(std::string const &error) {
    fprintf(stderr, "ERROR: %s\n", error.c_str());
}

void show_usage() {
    printf("Usage: [port]\n"
           "by default port is 8080\n\n");
}

int open_socket() {
    int listener = socket(AF_INET, SOCK_STREAM, 0);

    if (listener == -1) {
        show_err("Cannot open socket");
        exit(EXIT_FAILURE);
    }

    return listener;
}

void start(int listener, int port) {
    sockaddr_in serverAddress{};
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);

    if (bind(listener, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(sockaddr_in)) < 0) {
        show_err("Cannot bind");
        close(listener);
        exit(EXIT_FAILURE);
    }

    if (listen(listener, ATTEMPT) == -1) {
        show_err("Cannot listen");
        close(listener);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < ATTEMPT; ++i) {
        sockaddr_in client{};
        socklen_t clientSize;

        int accepted_fd = accept(listener, reinterpret_cast<sockaddr*>(&client), &clientSize);
        if (accepted_fd == -1) {
            show_err("Cannot accept");
            close(accepted_fd);
            exit(EXIT_FAILURE);
        }


        char buffer[BUFF_SIZE] = {};

        int readed = read(accepted_fd, buffer, BUFF_SIZE);
        
        if (readed == -1) {
            show_err("Cannot read");
            close(accepted_fd);
            exit(EXIT_FAILURE);
        }

        if (send(accepted_fd, buffer, readed, 0) != readed) {
            show_err("Cannot send");
        }

        close(accepted_fd);

    }

}

int main(int argc, char** argv) {
    if (argc > 2) {
        show_usage();
    }
    int port = PORT;

    if (argc == 2) {
        port = atoi(argv[1]);
    }

    int listener = open_socket();

    start(listener, port);

    close(listener);

    exit(EXIT_SUCCESS);
}