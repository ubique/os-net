#include <iostream>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "usage : ./server addr port";
        exit(EXIT_FAILURE);
    }

    int port;
    try {
        port = std::stoi(argv[2]);
    } catch (std::invalid_argument &e) {
        std::cout << "Bad port, can't convert to int";
        exit(EXIT_FAILURE);
    }

    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd == -1) {
        perror("Can't create socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    if (inet_pton(AF_INET, argv[1], &server.sin_addr) != 1) {
        perror("Bad adress");
        exit(EXIT_FAILURE);
    }

    std::cout << "Enter your name and mister Server will say hello to you!\n";
    std::cout << "Enter stop if you want to stop!\n";

    if (connect(fd, (sockaddr *) (&server), sizeof(server)) == -1) {
        perror("Can't connect.");
        exit(EXIT_FAILURE);
    }

    std::string message;
    size_t const BUFFER_SIZE = 1000;
    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);

    bool working = true;
    while (working) {
        bzero(buffer, BUFFER_SIZE);
        std::cin >> message;

        if (message == "stop") {
            working = false;
        }

        message += 3;

        size_t sended = 0;
        while (sended < message.size()) {
            ssize_t curr_portion = send(fd, message.data() + sended, message.length() - sended, 0);
            if (curr_portion == -1) {
                perror("Can't send message.");
                continue;
            }
            sended += curr_portion;
        }

        size_t received = 0;
        char last_received = 'a';

        while (last_received != 3) {
            ssize_t len = recv(fd, buffer, BUFFER_SIZE, 0);
            if (len == -1) {
                perror("Can't read response./");
                continue;
            }
            if (len == 0) {
                break;
            }
            received += len;
            last_received = buffer[len - 1];
        }

        buffer[received - 1] = 0;

        std::cout << buffer << '\n';
    }

    close(fd);
}